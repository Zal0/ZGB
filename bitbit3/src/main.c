#include <gb/gb.h> 

#include "OAMManager.h"
#include "Frame.h"
#include "Sprite.h"
#include "Keys.h"
#include "Scroll.h"

#include <gb/drawing.h>

#include "string.h"

void Start() {
}

void Update() {
}

void main() {
	Start();
	while (1) {
		wait_vbl_done();
		
		ResetOAM();
		UPDATE_KEYS();

		Update();
		RefreshScroll();
		FlushOAM();
	}
}
