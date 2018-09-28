// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

	@color
	M=0   // white by default

(EVENT_LOOP)
	@KBD
	D=M
	// if kbd_value != 0, key is pressed.
	@KEY_PRESSED
	//0; JMP  // todo, remove me
	D; JNE

	// else key not pressed
	@KEY_NOT_PRESSED
	0; JMP

(KEY_PRESSED)
	// if color == -1, just go back to loop, no need to color
	// screen again.
	@color
	D=M+1
	@EVENT_LOOP
	D; JEQ

	// else color was != -1, so redraw screen.
	@color
	M=-1   // black color
	@COLOR_SCREEN
	0; JMP

(KEY_NOT_PRESSED)
	// if color == 0, just go back to loop, no need to color
	// screen with white since it's already white.
	@color
	D=M
	@EVENT_LOOP
	D; JEQ

	// else color was != 0, so redraw screen.
	@color
	M=0
	@COLOR_SCREEN
	0; JMP

(COLOR_SCREEN)
	// i = SCREEN block start
	@SCREEN
	D=A
	@i
	M=D

	// end = i + 8192 (8K) ; or could have used end = @kbd, but that's more hackish
	@8192
	D=D+A
	@end
	M=D

	// while i < end ; do write in display memory map at position i
(COLOR_SCREEN_LOOP)
	// if i >= end, goto screen done
	@i
	D=M
	@end
	D=D-M
	@COLOR_SCREEN_DONE
	D; JGE

	// color screen at position i
	@color
	D=M
	@i
	A=M   // go to address i
	M=D

	// i++
	@i
	M=M+1

	// repeat
	@COLOR_SCREEN_LOOP
	0; JMP

(COLOR_SCREEN_DONE)
	// jump back into event loop
	@EVENT_LOOP
	0; JMP
