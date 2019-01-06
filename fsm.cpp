//
// fsm.cpp
//

#include "fsm.hpp"

using namespace std;

FSM::FSM() {
  this -> state = -1;
  this -> default_state = -1;
}

int FSM::addState(string label, bool is_accept_state) {
  State* new_state  = new State;
  new_state -> label = label;
  new_state -> accept = is_accept_state;
  new_state -> failure_trans = -1;
  this -> states.push_back(new_state);
  if (this -> states.size() == 1){
    this -> state = 0;
    this -> default_state = 0;
  }
  return this->states.size() -1;
}

// addState adds a state with the given label. This is equivalent to
// addState(label, false). See the other addState function for
// remaining documentation.
int FSM::addState(string label) {
  return addState(label,false);
}

int FSM::addTransition(int stateA, int stateB, 
		       int signal, string transLabel) {
  // this method's documentation is longer than the implementation. to
  // help out, I'm including the comments for my implementation.
  //
  // 1. if stateA or stateB is out of range for the states vector,
  // return -1.
  if (stateA < 0 || stateA > this -> states.size() - 1 || stateB < 0 || stateB > this -> states.size() - 1){
    return -1;
  } else if (states[stateA] == NULL || states[stateB] == NULL){
    return -1;
  }

  // 3. build a new Transition, and set its values.
  Transition* new_transition = new Transition;
  new_transition -> label = transLabel;
  new_transition -> signal = signal;
  new_transition -> next_state = stateB;

  // 2. get the State* for stateA and look for a duplicate
  // transition. if the new transition is FAILURE_SIGNAL, only need to
  // check the state's `failure_trans`. if it is a regular signal,
  // need to look at each id in the state's `trans` vector. if there's
  // a duplicate, return -1.
  State* tmpA = this -> states[stateA];
  for(int t = 0 ; t != tmpA -> trans.size() ; ++t){
    int tmp_id = tmpA ->trans[t];
    if (getTransition(tmp_id) -> next_state == new_transition -> next_state){
      return -1;
    }
  }

  // 4. add the new transition to the FSM and get its ID.
  // 5. using the State* from step 2, use the new transition ID to
  // either set the failure_trans or add to the trans list.
  tmpA -> trans.push_back(transitions.size());
  this -> transitions.push_back(new_transition);

  // 6. return the new transition's ID.
  return transitions.size()-1 ;
}

int FSM::countStates() {
  cout << "CountStates:  " << this -> states.size() << endl;
  return this -> states.size();;
}

int FSM::countTransitions() {
  cout << "CountTransitions:  " << this -> transitions.size() << endl;
  return this -> transitions.size();
}

int FSM::getCurrentState() {
  cout << "getCurrentState: " << this -> state << endl;
  return this -> state;
}

bool FSM::isAcceptState() {
  if (states.size() == 0 || states[state] -> accept == false){
    return false;
  }
  return true;
}

State* FSM::getState(int id) {
  if( id < 0 || id >= states.size()){
      return nullptr;
  }
  return states[id];
}

Transition* FSM::getTransition(int id) {
  if(id < 0 || id >= transitions.size()){
    return nullptr;
  }
  return transitions[id];
}

int FSM::getDefaultState(){
  cout << "getDefaultState  " << this -> default_state << endl;
  return this -> default_state;
}

void FSM::setState(int id) {
  state = id;
}

bool FSM::handleSignal(int signal) {
  //
  // 1. If the FSM is currently in a bad state, return false.
  State* current_state = getState(state);
  if (current_state == NULL){
    return false;
  }
  
  // 2. Iterate through the current state's normal transitions and
  // look for one that has the same signal as the one given to the
  // method.
  for (int i = 0; i != current_state->trans.size() ; ++i){
    int tmp_id = current_state->trans[i];
    //Transition* tr = getTransition(tmp_id);
    // 4. If there was a normal or a failure transition, update the FSM
    // to be in the next_state indicated by that transition, and return
    // true.
    if (getTransition(tmp_id) -> signal == signal){
      state = getTransition(tmp_id)->next_state;
      cout << "STATE IS: " << getState(getCurrentState()) << endl;
      return true;
    } else {
      current_state -> failure_trans = tmp_id;
    }
  }
  // 3. If there wasn't a normal transition, see if there was a
  // failure_trans transition for the state. If not, return false.
  if (current_state->failure_trans != -1){
    //setState(st->failure_trans);
    state = getTransition(current_state->failure_trans)->next_state;
    cout << "STATE IS: " << getState(getCurrentState()) << endl;
    if (current_state -> label == "No Input"){
      return true;
    }
  }  

  return false;
}


ostream &operator << (ostream& out, FSM* fsm) {
  int c = 0;
  for (auto it=fsm->states.begin(); it != fsm->states.end(); ++it) {
    out << "  state " << c << ": " << *it << endl;
    c++;
  }
  c = 0;
  for (auto it=fsm->transitions.begin(); it != fsm->transitions.end(); ++it) {
    out << "  trans " << c << ": " << *it << endl;
    c++;
  }
  out << "  current state: " << fsm->state;
  if (fsm->isAcceptState()) {
    out << " (accepting state!)";
  }
  out << endl;
  return out;
}

ostream &operator << (ostream& out, Transition* tr) {
  if (tr == NULL) {
    out << "Transition NULL";
  } else {
    out << "\"" << tr->label << "\" (" << tr->signal << ") --> " << tr->next_state;
  }
  return out;
}

ostream &operator << (ostream& out, State* st) {
  if (st == NULL) {
    out << "State: NULL";
  } else {
    out << "State: " << st->label;
    if (st->accept) {
      out << " +";
    }
  }
  return out;
}
