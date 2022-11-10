#ifndef SERVO_WRAP_H
#define SERVO_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif

void servo_init(void);
void servo_write(int val);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ! SERVO_WRAP_H