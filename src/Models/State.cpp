// #include "State.h"


// // const State state_list[] = {
// //     State(STATES::IDLE, )
// // };




// State::State(STATES state, std::function<esp_err_t()> action, std::map<STATES, std::function<bool()>> transitions): _state(state), _transitions(transitions), _action(action){

// }

// // Returns the state mapped to the first true transition or its own state if None
// STATES State::check_transitions(){

//     if(ESP_OK != _status){
//         return STATES::ERROR;
//     }

//     for( unsigned int i = 0; i < sizeof(_transitions)/sizeof(_transitions[0]); i = i + 1 ){
//         if(_transitions[i].condition()){
//             return _transitions[i].next_state;
//         }
//     }

//     return _state;
// }

// esp_err_t State::execute(){
//     _status = _action();
//     return _status;
// }