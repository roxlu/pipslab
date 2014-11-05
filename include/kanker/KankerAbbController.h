/*
  
  Kanker ABB Controller
  ----------------------

  This class ties everything related to controlling the ABB
  together. It uses a `KankerFont` instance to generate the 
  draw commands for the ABB. It uses `Ftp` to upload a new set
  of commands to the ABB which will be read by the application 
  running on the ABB.  We poll the ABB to check if the state 
  changed. 


  Make sure to link with the following libraries:
  ----------------------------------------------
  - curl
  - ldap (for curl)
  

 */
#ifndef KANKER_ABB_CONTROLLER_H
#define KANKER_ABB_CONTROLLER_H

#include <stdint.h>
#include <vector>
#include <fstream>
#include <string>
#include <kanker/Ftp.h>
#include <kanker/KankerFont.h>
#include <kanker/KankerAbb.h>
#include <rapidxml.hpp>

using namespace rapidxml;

/* ----------------------------------------------------------------- */

class KankerAbbControllerSettings {
 public:
  std::string font_file;
  std::string settings_file;
};

/* ----------------------------------------------------------------- */

enum KankerAbbControllerState {
  KC_STATE_NONE,                 /* Initial state when not initialized. */
  KC_STATE_READY,                /* We're ready and waiting for a new message from the user. We can directly upload the new message. */
  KC_STATE_WRITING,              /* The ABB is currently writing the message. */
};

/* ----------------------------------------------------------------- */

class KankerAbbControllerListener {
 public:
  virtual void onAbbStateChanged(int state, int64_t messageID) {};
};

/* ----------------------------------------------------------------- */

class KankerAbbController {

 public:
  KankerAbbController();
  ~KankerAbbController();
  int init(KankerAbbControllerSettings cfg, KankerAbbControllerListener* listener);             /* Initialize the controller. */
  int writeText(int64_t id, std::string text);                                                  /* This make sure that the ABB will draw the given text */  
  void update();                                                                                /* Call this often to make sure that we can read/update the remote state. */
                                                                                                
  int checkAbbState();                                                                          /* This will download the ABB state. */
  int parseStateXml(std::string str);                                                           /* We use a very basic way to keep state of the ABB. We poll the ABB and downlaod a state file */
  void switchState(int st);                                                                     /* Used internally to switch between states based on the ABBs state. */ 
                                                                                                
 public:                                                                                        
  KankerAbbControllerListener* listener;
  KankerAbbControllerSettings settings;                                                         
  KankerFont kanker_font;                                                                       
  KankerAbb kanker_abb;                                                                         
  Ftp ftp;                                                                                      
  std::vector<KankerAbbGlyph> abb_glyphs;                                                       /* Storage for the glyphs that are generated by the KankerAbb and KankerFont objects. */
  std::vector<std::vector<vec3> > abb_points;                                                   /* Storage for the glyphs that are generated by the KankerAbb; we don't actually use them here but they may be used to draw line segments that make up the font. */
  int is_init;                                                                                  
  int state;                                                                                    /* The current state of the controller. */
  int64_t last_message_id;                                                                      
  uint64_t poll_delay;                                                                          /* Delay in ns. */
  uint64_t poll_timeout;                                                                        /* The timeout when we should start connect to the ABB and check it's state (in nanosec. ) */ 
}; 

#endif
