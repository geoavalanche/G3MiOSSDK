//
//  Downloader.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 26/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Downloader.hpp"

#include "Storage.hpp"

#include "INetwork.hpp"

Downloader::Downloader(Storage *storage, unsigned int maxSimultaneous, INetwork * const net):
_storage(storage), _maxSimultaneous(maxSimultaneous), _network(net), _simultaneousDownloads(0)
{
}

void Downloader::request(std::string& urlOfFile, int priority, IDownloadListener *listener)
{
  
  for (int i = 0; i < _petitions.size(); i++)
  {
    if (urlOfFile == _petitions[i]._url){ //IF WE FOUND THE SAME PETITION
      
      if (priority > _petitions[i]._priority){ //MAX PRIORITY
        _petitions[i]._priority = priority;
      }
      _petitions[i].addListener(listener); //NEW LISTENER
      return;
    }
  }
  
  //NEW DOWNLOAD
  Download d(urlOfFile, priority, listener);
  _petitions.push_back(d);
  
  //When a new petition comes, we try to throw a download
  startDownload();
}

void Downloader::startDownload()
{
  if (_maxSimultaneous > _simultaneousDownloads) { //If we are able to throw more dowloads
    
    //Selecting download
    int maxPrior = -99999999;
    int downloadIndex = -1;
    for (int i = 0; i < _petitions.size(); i++)
    {
      if (_petitions[i]._priority > maxPrior){
        maxPrior = _petitions[i]._priority;
        downloadIndex = i;
      }
    }
    if (downloadIndex < 0) return;
    
    //Downloading
    _network->request(_petitions[downloadIndex]._url, this);
    
    //One more in the net
    _simultaneousDownloads++;
  }
}

void Downloader::onDownload(const Response& e)
{
  for (int i = 0; i < _petitions.size(); i++)
  {
    if (_petitions[i]._url == e.url.path) //RECEIVED RESPONSE
    {
      Download& pet = _petitions[i];
      for (int j = 0; j < pet._listeners.size(); j++) {
        pet._listeners[j]->onDownload(e);
      }
      
      _petitions.erase(_petitions.begin() + i);
      
      return;
    }
  }
  
}

void Downloader::onError(const Response& e)
{
  for (int i = 0; i < _petitions.size(); i++)
  {
    if (_petitions[i]._url == e.url.path) //RECEIVED RESPONSE
    {
      Download& pet = _petitions[i];
      for (int j = 0; j < pet._listeners.size(); j++) {
        pet._listeners[j]->onError(e);
      }
      
      _petitions.erase(_petitions.begin() + i);
      
      return;
    }
  }
}