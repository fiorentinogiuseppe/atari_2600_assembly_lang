	processor 6502

	include "vcs.h"
	include "macro.h"

	seg code
	org $F000		; defines the origin of the ROM at $F000

START:
	;CLEAN_START		; Macro to safely clear the memory

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Set bg luminosity color to yellow
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	lda #$1E		; Load color into A ($1E is NTSC yellow)
	sta COLUBK		; store A to BakgroundColor Address $09

	jmp START		; Repeate from START

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Fill ROM size to exectly 4kb
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	org $FFFC		; Defines origin to $FFFC
	.word START		; Reset vector at $FFFC ( where program starts)
	.word START		; Interrupt vector at $FFFE (unused in the VCS)

