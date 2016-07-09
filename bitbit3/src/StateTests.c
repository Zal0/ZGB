#include "StateTests.h"

#include <gb/gb.h>

#include "Vector.h"
#include <stdio.h>

DECLARE_VECTOR(v_test, 10)

void StartStateTests() {
	UINT8 i;

	VectorAdd(v_test, 0u);
	VectorAdd(v_test, 3u);
	VectorAdd(v_test, 2u);
	VectorAdd(v_test, 1u);
	PRINT_VECTOR(v_test);

	VectorRemovePos(v_test, 2);
	PRINT_VECTOR(v_test);

	VectorRemovePos(v_test, 0);
	PRINT_VECTOR(v_test);

	VectorAdd(v_test, 5);
	PRINT_VECTOR(v_test);
}

void UpdateStateTests() {

}