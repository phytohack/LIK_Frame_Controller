# Frame Controller - Main Controller <!-- omit in toc -->

- [Main Controller -\> Frame Controller](#main-controller---frame-controller)
    - [Структура сообщения при `msg_type` = *'device\_command'*](#структура-сообщения-при-msg_type--device_command)
    - [Структура сообщения при `msg_type` = *'device\_state\_request'*](#структура-сообщения-при-msg_type--device_state_request)
    - [Структура сообщения при `msg_type` = *'identity\_responce'*](#структура-сообщения-при-msg_type--identity_responce)
- [Main Controller \<- Frame Controller](#main-controller---frame-controller-1)
    - [Структура сообщения при `msg_type` = *'identity\_request'*](#структура-сообщения-при-msg_type--identity_request)
    - [Структура сообщения при `msg_type` = *'device\_command\_responce'*](#структура-сообщения-при-msg_type--device_command_responce)
    - [Структура сообщения при `msg_type` = *'device\_state'*](#структура-сообщения-при-msg_type--device_state)


Frame Controller выступает сервером,
Main Controller - клиентом.

### Main Controller -> Frame Controller
##### Обязательные поля: <!-- omit in toc -->
- `msg_id` : `int` 
- `msg_type` :
  - [*device_command*](#структура-сообщения-при-msg_type--device_command)
  - [*device_state_request*](#структура-сообщения-при-msg_type--device_state_request)
  - [*identity_response*](#структура-сообщения-при-msg_type--identity_responce)

#### Необязательные поля: <!-- omit in toc -->
- `responce_required` : *true* | *false*


##### Структура сообщения при `msg_type` = *'device_command'*
```json
{
    "msg_id"            : 222,
    "msg_type"          : "device_command",
    "device_type"       : "stepper",
    "device_name"       : "thermal_camera_stepper",
    "command"           : "go_to_x" | "basing" | "precise_basing" | "stop",
    // при command = "go_to_x":
    "x"                 : 5000,
    "speed"             : 3000, // необяз. поле
    "responce_required" : true
}
```

##### Структура сообщения при `msg_type` = *'device_state_request'*
```json
{
    "msg_id"            : 222,
    "msg_type"          : "device_state_request",
    "device_type"       : "stepper",
    "device_name"       : "thermal_camera_stepper"
}
```

##### Структура сообщения при `msg_type` = *'identity_responce'*
```json
{
    "msg_id"            : 111,
    "msg_type"          : "identity_responce",
    "role"              : "main_controller"
}
```


### Main Controller <- Frame Controller

##### Обязательные поля <!-- omit in toc -->
- `msg_id` : `int`
- `msg_type` : 
  - [*identity_request*](#структура-сообщения-при-msg_type--identity_request) 
  - [*device_command_responce*](#структура-сообщения-при-msg_type--device_command_responce)
  - [*device_state*](#структура-сообщения-при-msg_type--device_state)

##### Необязательные поля <!-- omit in toc -->
- `responce_required` : *true* | *false*

##### Структура сообщения при `msg_type` = *'identity_request'*
```json
{
    "msg_id"            : 333,
    "msg_type"          : "identity_request"
}
```

##### Структура сообщения при `msg_type` = *'device_command_responce'*
```json
{
    "msg_id"            : 111,
    "request_msg_id"    : 222,
    "msg_type"          : "device_command_responce",
    "status"            : "success" | "error",
    // при status = error:
    "error_msg"         : "error description"
}
```

##### Структура сообщения при `msg_type` = *'device_state'*
```json
{
    "msg_id"            : 222,
    "msg_type"          : "device_state",
    "device_type"       : "stepper",
    "device_name"       : "thermal_camera_stepper",
    "state"             : "hold",
    "abs_position"      : "base",
    "x"                 : 1000
}
```
state: функция StepperStateNameJSON
abs_position: функция StepperPositionNameJSON(StepperPosition position)
