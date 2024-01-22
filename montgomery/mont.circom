pragma circom 2.0.0;

include "../../circomlib/circuits/montgomery.circom";

template Main() {
    signal input in1[2];
    signal input in2[2];
    signal output out[2];

    component mont = MontgomeryAdd();

    mont.in1[0] <== in1[0];
    mont.in1[1] <== in1[1];

    mont.in2[0] <== in2[0];
    mont.in2[1] <== in2[1];

    out[0] <== mont.out[0];
    out[1] <== mont.out[1];

}

component main = Main();


// template Main(n, k) {
//     signal input privkey[k];
//     signal output privkeyHash[k];

//     component sha256 = Sha256(256);

//     for (var i = 0; i < k; i++) {
//         for (var j =0; j < n; j++) {
//         // change privkey to big-endian as sha256 input
//         sha256.in[i * n + j] <-- (privkey[k-1-i] >> (n-1-j)) & 1;
//         }
//     }

//     component b2n[k];
//     for (var i = 0; i < k; i++) {
//         b2n[i] = Bits2Num(n);
//         for(var j = 0; j < n; j++) {
//         // `b2n` input is little-endian in bits, `sha256` out is big-endian in bits
//         b2n[i].in[n-1-j] <== sha256.out[i * n + j];
//         }
//         privkeyHash[i] <== b2n[i].out;
//     }

// }

// component main = Main(1, 256);