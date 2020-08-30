#pragma once
extern char layout[60];// = "1234567890-=<>qwertyuiop[]?\\asdfghjkl;'!@#zxcvbnm,./";
extern char buffer[200];// = "";
enum KeyState {
	Up, Down, Pressed, Released
};

extern bool shiftKey;