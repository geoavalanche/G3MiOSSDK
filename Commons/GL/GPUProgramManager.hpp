//
//  GPUProgramManager.h
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 02/04/13.
//
//

#ifndef __G3MiOSSDK__GPUProgramManager__
#define __G3MiOSSDK__GPUProgramManager__

#include <iostream>

#include <map>
#include <string>

#include "GPUProgramFactory.hpp"

#include "GPUProgram.hpp"
#include "GL.hpp"
#include "GPUProgramState.hpp"

class GPUProgramManager{
  
  std::map<std::string, GPUProgram*> _programs;
  
  GPUProgramFactory *_factory;
public:
  
  GPUProgramManager(GPUProgramFactory *factory):_factory(factory){}
  
  ~GPUProgramManager(){
    
#ifdef C_CODE
    delete _factory;
    for (std::map<std::string, GPUProgram*>::iterator it = _programs.begin(); it != _programs.end(); ++it){
      delete it->second;
    }
#endif
  }
  
  GPUProgram* getCompiledProgram(const std::string& name){
#ifdef C_CODE
    std::map<std::string, GPUProgram*>::iterator it = _programs.find(name);
    if (it != _programs.end()){
      return it->second;
    } else{
      return NULL;
    }
#endif
#ifdef JAVA_CODE
    return _programs.get(name);
#endif
  }
  
  GPUProgram* getProgram(GL* gl, const std::string& name){
    
    GPUProgram* prog = getCompiledProgram(name);
    if (prog == NULL){
      const GPUProgramSources* ps = _factory->get(name);
      
      //Compile new Program
      if (ps != NULL){
        prog = GPUProgram::createProgram(gl,
                                         ps->_name,
                                         ps->_vertexSource,
                                         ps->_fragmentSource);
        if (prog == NULL){
          ILogger::instance()->logError("Problem at creating program named %s.", name.c_str());
          return NULL;
        }
        
        _programs[name] = prog;
      }
      
    }
    return prog;
  }
  
//  GPUProgram* getProgram(GL* gl, const GPUProgramState& state) {
//#ifdef C_CODE
//    for(std::map<std::string, GPUProgram*>::const_iterator it = _programs.begin();
//        it != _programs.end(); it++){
//      if (state.isLinkableToProgram(*it->second)){
//        return it->second;
//      }
//    }
//#endif
//#ifdef JAVA_CODE
//    for (final GPUProgram p : _programs.values()){
//    	if (state.isLinkableToProgram(p)) {
//        return p;
//      }
//    }
//#endif
//    
//    int WORKING_JM;
//    
//    std::vector<std::string>* us = state.getUniformsNames();
//    int size = us->size();
//    for (int i = 0; i < size; i++) {
//      if (us->at(i).compare("ViewPortExtent") == 0){
//        return getProgram(gl, "Billboard");
//      }
//    }
//    
//    return getProgram(gl, "Default");
//  }
  
  GPUProgram* getProgram(GL* gl, const GLState* glState);
  
  
};

#endif /* defined(__G3MiOSSDK__GPUProgramManager__) */
