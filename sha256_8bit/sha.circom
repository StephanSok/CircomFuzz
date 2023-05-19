pragma circom 2.0.0;

include "../../circomlib/circuits/sha256/sha256.circom";

template Main(nbits) {
    signal input in[nbits];
    signal output out[256];

    component sha256 = Sha256(nbits);
    var k;
    for (k = 0; k < nbits; k++) {
        sha256.in[k] <== in[k];
    }

    for (k = 0; k < 256; k++) {
        out[k] <== sha256.out[k];
    }
}

component main = Main(8);

// pragma circom 2.0.0;

// include "../../circomlib/circuits/sha256/sha256.circom";
// include "../../circomlib/circuits/bitify.circom";

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


// pragma circom 2.0.0;

// include "../../circomlib/circuits/sha256/sha256.circom";

// template Main(nBits) {
//     signal input in[nBits];
    

//      var i;
//     var k;
//     var nBlocks;
//     var bitsLastBlock;


//     nBlocks = ((nBits + 64)\512)+1;

//     signal paddedIn[nBlocks*512];
//     signal output out[nBlocks*512];

//     for (k=0; k<nBits; k++) {
//         paddedIn[k] <== in[k];
//     }
//     paddedIn[nBits] <== 1;

//     for (k=nBits+1; k<nBlocks*512-64; k++) {
//         paddedIn[k] <== 0;
//     }

//     for (k = 0; k< 64; k++) {
//         paddedIn[nBlocks*512 - k -1] <== (nBits >> k)&1;
//     }
//     for (k = 0; k < nBlocks*512; k++) {
//         out[k] <== paddedIn[k];
//     }
// }

// component main = Main(1);

// pragma circom 2.0.0;

// include "../../circomlib/circuits/sha256/sha256_2.circom";

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