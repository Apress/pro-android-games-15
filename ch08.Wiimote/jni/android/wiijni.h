#ifndef JNI_H
#define JNI_H

#ifdef __cplusplus
extern "C" {
#endif

void dump_wiimotes();

void jni_send_message(char *format, ...);
void jni_send_error(int code, char *format, ...);
void jni_send_event(int wiimote, char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
