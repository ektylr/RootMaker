// VIDEmbedder.cc
// embeds electron and photon ids
// embeds electron_cutBased*
// embeds electron_mvaNonTrig*
// embeds electron_mvaTrig*

#include <memory>
#include <vector>
#include <iostream>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "FWCore/Framework/interface/MakerMacros.h"

using namespace std;

template<typename T>
class VIDEmbedder : public edm::stream::EDProducer<>
{
  public:
    explicit VIDEmbedder(const edm::ParameterSet&);
    ~VIDEmbedder() {}

  private:
    void beginJob() {}
    virtual void produce(edm::Event &iEvent, const edm::EventSetup &iSetup);
    void endJob() {}

    edm::EDGetTokenT<edm::View<T> > collectionToken_;
    vector<string> idLabels_;
    vector<edm::EDGetTokenT<edm::ValueMap<bool> > > idMapTokens_;
    vector<string> valueLabels_;
    vector<edm::EDGetTokenT<edm::ValueMap<float> > > valueTokens_;
    vector<string> categoryLabels_;
    vector<edm::EDGetTokenT<edm::ValueMap<int> > > categoryTokens_;
    auto_ptr<vector<T> > out;
};

template<typename T>
VIDEmbedder<T>::VIDEmbedder(const edm::ParameterSet &iConfig):
    collectionToken_(consumes<edm::View<T> >(iConfig.getParameter<edm::InputTag>("src"))),
    idLabels_(iConfig.exists("idLabels") ? iConfig.getParameter<vector<string> >("idLabels") : vector<string>()),
    valueLabels_(iConfig.exists("valueLabels") ? iConfig.getParameter<vector<string> >("valueLabels") : vector<string>()),
    categoryLabels_(iConfig.exists("categoryLabels") ? iConfig.getParameter<vector<string> >("categoryLabels") : vector<string>())
{
    vector<edm::InputTag> idTags = iConfig.getParameter<vector<edm::InputTag> >("ids");
    for(unsigned int i = 0; (i < idTags.size() && i < idLabels_.size()); ++i) {
        idMapTokens_.push_back(consumes<edm::ValueMap<bool> >(idTags.at(i)));
    }
    vector<edm::InputTag> valueTags = iConfig.getParameter<vector<edm::InputTag> >("values");
    for(unsigned int i = 0; (i < valueTags.size() && i < valueLabels_.size()); ++i) {
        valueTokens_.push_back(consumes<edm::ValueMap<float> >(valueTags.at(i)));
    }
    vector<edm::InputTag> categoryTags = iConfig.getParameter<vector<edm::InputTag> >("categories");
    for(unsigned int i = 0; (i < categoryTags.size() && i < categoryLabels_.size()); ++i) {
        categoryTokens_.push_back(consumes<edm::ValueMap<int> >(categoryTags.at(i)));
    }
    produces<vector<T> >();
}

template<typename T>
void VIDEmbedder<T>::produce(edm::Event &iEvent, const edm::EventSetup &iSetup)
{
    out = auto_ptr<vector<T> >(new vector<T>);
    edm::Handle<edm::View<T> > collection;
    vector<edm::Handle<edm::ValueMap<bool> > > ids(idMapTokens_.size(), edm::Handle<edm::ValueMap<bool> >() );
    vector<edm::Handle<edm::ValueMap<float> > > values(valueTokens_.size(), edm::Handle<edm::ValueMap<float> >() );
    vector<edm::Handle<edm::ValueMap<int> > > categories(categoryTokens_.size(), edm::Handle<edm::ValueMap<int> >() );
    iEvent.getByToken(collectionToken_, collection);
    for (unsigned int i = 0; i < idMapTokens_.size(); ++i) {
        iEvent.getByToken(idMapTokens_.at(i), ids.at(i));
    }
    for(unsigned int i = 0; i < valueTokens_.size(); ++i) {
        iEvent.getByToken(valueTokens_.at(i), values.at(i));
    }
    for(unsigned int i = 0; i < categoryTokens_.size(); ++i) {
        iEvent.getByToken(categoryTokens_.at(i), categories.at(i));
    }
    for (size_t c = 0; c < collection->size(); ++c) {
       const auto obj = collection->at(c);
       const auto ptr = collection->ptrAt(c);
       T newObj = obj;
       for(unsigned int i = 0; i < ids.size(); ++i) {
           bool result = (*(ids.at(i)))[ptr];
           newObj.addUserInt(idLabels_.at(i), result);
       }
       for(unsigned int i = 0; i < values.size(); ++i) {
           float result = (*(values.at(i)))[ptr];
           newObj.addUserFloat(valueLabels_.at(i), result);
       }
       for(unsigned int i = 0; i < categories.size(); ++i) {
           int result = (*(categories.at(i)))[ptr];
           newObj.addUserInt(categoryLabels_.at(i), result);
       }
       out->push_back(newObj);
    }
    iEvent.put(out);
}

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

typedef VIDEmbedder<pat::Electron> ElectronVIDEmbedder;
typedef VIDEmbedder<pat::Photon> PhotonVIDEmbedder;

DEFINE_FWK_MODULE(ElectronVIDEmbedder);
DEFINE_FWK_MODULE(PhotonVIDEmbedder);
