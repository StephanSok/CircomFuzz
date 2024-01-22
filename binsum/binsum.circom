pragma circom 2.0.0;

include "../../circomlib/circuits/binsum.circom";

template Main(n, ops) {
    var nout = nbits((2**n -1)*ops);
    signal input in[ops][n];
    signal output out[nout];

    component binsum = BinSum(n, ops);
    var k;
    var j;
    for (k=0; k<n; k++) {
        for (j=0; j<ops; j++) {
            binsum.in[j][k] <== in[j][k];
        }
    }

    for (k = 0; k < nout; k++) {
        out[k] <== binsum.out[k];
    }
}

component main = Main(64, 2);