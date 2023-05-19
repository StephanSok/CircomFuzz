// pragma circom 2.0.0;

// include "../circomlib/circuits/sha256/sha256_2.circom";

// template Main() {
//     signal input a;
//     signal input b;
//     signal output out;

//     component sha256_2 = Sha256_2();

//     sha256_2.a <== a;
//     sha256_2.b <== a;
//     out <== sha256_2.out;
// }

// component main = Main();

pragma circom 2.0.0;

include "/home/stepa/circomlib/circuits/bitify.circom";

template Main(n) {
    signal input in;
    signal output out[n];

    component n2b = Num2Bits(n);
    n2b.in <== in;
    var i;
    for (i = 0; i < n; i++) {
        out[i] <== n2b.out[i];
    }

}
component main = Main(70);