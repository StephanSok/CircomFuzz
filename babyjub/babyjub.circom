pragma circom 2.0.0;

include "../../circomlib/circuits/babyjub.circom";

template Main() {
    signal input in1[2];
    signal input in2[2];
    signal output out[2];

    component baby = BabyAdd();

    baby.x1 <== in1[0];
    baby.y1 <== in1[1];

    baby.x2 <== in2[0];
    baby.y2 <== in2[1];

    out[0] <== baby.xout;
    out[1] <== baby.yout;

}

component main = Main();