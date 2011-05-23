// BSD license

#include <stdio.h>
#include <stdlib.h>

// AKsv (transmitter) is 0x7612e6f168
// private key set is

unsigned long long hdcp_master[40][40] =
{{0x6692d179032205LL, 0xb4116a96425a7fLL, 0xecc2ef51af1740LL, 0x959d3b6d07bce4LL, 0xfa9f2af29814d9LL,
0x82592e77a204a8LL, 0x146a6970e3c4a1LL, 0xf43a81dc36eff7LL, 0x568b44f60c79f5LL, 0xbb606d7fe87dd6LL,
0x1b91b9b73c68f9LL, 0xf31c6aeef81de6LL, 0x9a9cc14469a037LL, 0xa480bc978970a6LL, 0x997f729d0a1a39LL,
0xb3b9accda43860LL, 0xf9d45a5bf64a1dLL, 0x180a1013ba5023LL, 0x42b73df2d33112LL, 0x851f2c4d21b05eLL,
0x2901308bbd685cLL, 0x9fde452d3328f5LL, 0x4cc518f97414a8LL, 0x8fca1f7e2a0a14LL, 0xdc8bdbb12e2378LL,
0x672f11cedf36c5LL, 0xf45a2a00da1c1dLL, 0x5a3e82c124129aLL, 0x084a707eadd972LL, 0xcb45c81b64808dLL,
0x07ebd2779e3e71LL, 0x9663e2beeee6e5LL, 0x25078568d83de8LL, 0x28027d5c0c4e65LL, 0xec3f0fc32c7e63LL,
0x1d6b501ae0f003LL, 0xf5a8fcecb28092LL, 0x854349337aa99eLL, 0x9c669367e08bf1LL, 0xd9c23474e09f70LL},

{0x3c901d46bada9aLL, 0x40981ffcfa376fLL, 0xa4b686ca8fb039LL, 0x63f2ce16b91863LL, 0x1bade89cc52ca2LL,
0x4552921af8efd2LL, 0xfe8ac96a02a6f9LL, 0x9248b8894b23bdLL, 0x17535dbff93d56LL, 0x94bdc32a095df2LL,
0xcd247c6d30286eLL, 0xd2212f9d8ce80aLL, 0xdc55bdc2a6962cLL, 0xbcabf9b5fcbe6fLL, 0xc2cfc78f5fdafaLL,
0x80e32223b9feabLL, 0xf1fa23f5b0bf0dLL, 0xab6bf4b5b698aeLL, 0xd960315753d36fLL, 0x424701e5a944edLL,
0x10f61245ebe788LL, 0xf57a17fc53a314LL, 0x00e22e88911d9eLL, 0x76575e18c7956eLL, 0xc1ef4eee022e38LL,
0xf5459f177591d9LL, 0x08748f861098efLL, 0x287d2c63bd809eLL, 0xe6a28a6f5d000cLL, 0x7ae5964a663c1bLL,
0x0f15f7167f56c6LL, 0xd6c05b2bbe8800LL, 0x544a49be026410LL, 0xd9f3f08602517fLL, 0x74878dc02827f7LL,
0xd72ef3ea24b7c8LL, 0x717c7afc0b55a5LL, 0x0be2a582516d08LL, 0x202ded173a5428LL, 0x9b71e35e45943fLL},

{0x9e7cd2c8789c99LL, 0x1b590a91f1cffdLL, 0x903dca7c36d298LL, 0x52ad58ddcc1861LL, 0x56dd3acba0d9c5LL,
0xc76254c1be9ed1LL, 0x06ecb6ae8ff373LL, 0xcfcc1afcbc80a4LL, 0x30eba7ac19308cLL, 0xd6e20ae760c986LL,
0xc0d1e59db1075fLL, 0x8933d5d8284b92LL, 0x9280d9a3faa716LL, 0x8386984f92bfd6LL, 0xbe56cd7c4bfa59LL,
0x16593d2aa598a6LL, 0xd62534326a40eeLL, 0x0c1f1919936667LL, 0xacbaf0eefdd395LL, 0x36dbfdbf9e1439LL,
0x0bd7c7e683d280LL, 0x54759e16cfd9eaLL, 0xcac9029104bd51LL, 0x436d1dca1371d3LL, 0xca2f808654cdb2LL,
0x7d6923e47f97b5LL, 0x70e256b741910cLL, 0x7dd466ed5fff2eLL, 0x26bec4a28e8cc4LL, 0x5754ea7219d4ebLL,
0x75270aa4d3cc8dLL, 0xe0ae1d1897b7f4LL, 0x4fe5663e8cb342LL, 0x05a80e4a1a950dLL, 0x66b4eb6ed4c99eLL,
0x3d7e9d469c6165LL, 0x81677af04a2e15LL, 0xada4be60bc348dLL, 0xdfdfbbad739248LL, 0x98ad5986f3ca1fLL},

{0x971d02ada31b46LL, 0x2adab96f7b15daLL, 0x9855f01b9b7b94LL, 0x6cef0f65663fbfLL, 0xeb328e8a3c6c5dLL,
0xe29f0f0b1ef2bfLL, 0xe4a30b29047d31LL, 0x52250e7ae3a4acLL, 0xfe3efc3b8c2df1LL, 0x8c997d15d6078bLL,
0x49da8b4611ff9fLL, 0xb1e061bc9be995LL, 0x31fd68c4ad6dc6LL, 0xfd8974f0c506ddLL, 0x90421c1cd2b26cLL,
0x53eec84c91ed17LL, 0x5159ba3711173bLL, 0x25e318ddceea6aLL, 0x98a14125755955LL, 0x2bb97fd341cea2LL,
0x3f8404769a0a8eLL, 0xbce5c7a45fb5d4LL, 0x9608307b43f785LL, 0x2a98e5856afe75LL, 0xb4dbead4815cacLL,
0xd1118af62c964aLL, 0x3142667a5b0d14LL, 0x6c6f90933acd3dLL, 0x6b14a0052e2be4LL, 0x1b1811fda0f554LL,
0x12300aa7f10405LL, 0x1919ca0bff56eaLL, 0xd3e2f3aad5250cLL, 0x4aeeea5101d2ecLL, 0x377fc499c07057LL,
0x6cb1a90cdb7b11LL, 0x3c839d47a4b814LL, 0x25c5ac14b5ec28LL, 0x4ef18646d5b9c2LL, 0x95a98cc51ebd3bLL},

{0x310e98028e24deLL, 0x092ffc76b79f44LL, 0x0740a1ca2d4737LL, 0xb9f38966257c99LL, 0xa75afc7454abe4LL,
0xa6dd815be8ccbfLL, 0xec2cac2df0c675LL, 0x41f7636aa4080fLL, 0x30e87b712520fdLL, 0xd5dfdc6d3266acLL,
0xee28f5479f836fLL, 0x0bf8ee2112173fLL, 0x43ae802fa8d52dLL, 0x4e0dffd36c1eacLL, 0x3cbda974bb7585LL,
0xfb60a4700470e3LL, 0xd9f6b6083ef13dLL, 0x4a5840f02d0130LL, 0x6c20ef5e35e2bfLL, 0xdad2f85c745b5bLL,
0x61c5ddc65d3fc9LL, 0x7f6ec395d4ae22LL, 0x2b8906fb3996e2LL, 0xe4110f59eb92acLL, 0x1cb212b44128bbLL,
0x545afda80a4fd1LL, 0xb1ffea547eab6bLL, 0xfac3d9166afce8LL, 0x3fe35fe17586f2LL, 0x9d082667026a4cLL,
0x17ffaf1cb50145LL, 0x24f27b316acfffLL, 0xb6bb758ec4ad60LL, 0x995e8726359ef7LL, 0xc44952cb424035LL,
0x5ec53461dbd248LL, 0x40a1586f04aee7LL, 0x49ea3fa4474e52LL, 0xc13e8f52c51562LL, 0x30a1a70162cfb8LL},

{0xccbada27b91c33LL, 0x33661064d05759LL, 0x3388bb6315b036LL, 0x0380a6b43851fbLL, 0x0228dadb44ad3dLL,
0xb732565bc37841LL, 0x993c0d383cfaaeLL, 0x0bea49476758acLL, 0xaccc69dbfcde8bLL, 0xf416ab0474f022LL,
0x2b7dbcc3002502LL, 0x20dc4e67289e50LL, 0x0068424fde9515LL, 0x64806d59eb0c18LL, 0x9cf08fb2abc362LL,
0x8d0ee78a6cace9LL, 0xb6781bd504d105LL, 0xaf65fab8ee6252LL, 0x64a8f8dd8e2d14LL, 0xcb9d3354e06b5bLL,
0x53082840d3c011LL, 0x8e080bedab3c4cLL, 0xe30d722a455843LL, 0x24955a20397c17LL, 0x82495c1c5114e8LL,
0x656e71c31d813dLL, 0x1f0a6d291823a1LL, 0x6327f9534353faLL, 0xb89529c2f034fbLL, 0x70e9b12205c7b3LL,
0xa06c87969407a2LL, 0x520bfa2fe80f90LL, 0xda1efc3d345c65LL, 0x313936ec023811LL, 0xa8cc87128be2faLL,
0x4cd0e8645ee141LL, 0xbe7975519e2b63LL, 0x9543d23113c2a8LL, 0x3d87b0da033f22LL, 0xdf0464c704e9d4LL},

{0x7e1a30947e867eLL, 0x014ae464b37935LL, 0x5c4babf689fa4eLL, 0xc4aec0cb01cc35LL, 0x328c0e4a0230e4LL,
0xfdacb93b419594LL, 0x26deefc8a553e6LL, 0x6e75a2d790cb55LL, 0x2c4554518f7396LL, 0x94b77184cb145dLL,
0x95f883f620a8bbLL, 0xedff42866a2783LL, 0x7b4ee6304b711dLL, 0xed56e077a4b9fbLL, 0xc4e60e687ff6c3LL,
0x0cbf144b8f64d5LL, 0x023dd10a35edddLL, 0xbeaa3323e999c6LL, 0xd2e016b31c38c4LL, 0x8d2917a888f799LL,
0x18c3abd28e736bLL, 0x8d38e69b4966ccLL, 0x624db0143dd2e7LL, 0x5e2fa510f632b7LL, 0xee6e64d45b139aLL,
0xa1c6d852e74be7LL, 0x429843b9e6bb7eLL, 0xdb9ab07c8dc267LL, 0x9efa092299f071LL, 0xdcca9e0e61e960LL,
0x94406fac95f1d8LL, 0xd19122f3f88782LL, 0x1b11a662e9c83fLL, 0xd161fd6fb7f032LL, 0x89f7d984da9d48LL,
0xa3583fea45fe58LL, 0x885e2c4839e254LL, 0x47e87235f713b1LL, 0xf4732e05b71aeeLL, 0xae026d063f4349LL},

{0x0a481d2db197afLL, 0xabfce1039d4ac0LL, 0x4a6b89d2d1aeacLL, 0x0842eb7178cc53LL, 0xb82ce2835f1937LL,
0x3b4002ca21d6b6LL, 0xe64a78a78abb27LL, 0x8bd6142ad04526LL, 0xe035dacb23624aLL, 0x4cf80110135771LL,
0x7a52fafc92745eLL, 0xefa28a290ea782LL, 0x735617cd8b0221LL, 0xb095e9f4b286a5LL, 0x021e9ba0727645LL,
0x3e58e9ec16ed1cLL, 0xd7732bb5ba99a6LL, 0x374bde43fa89a9LL, 0xcb83e5ef2e4d04LL, 0x1da4f73566d134LL,
0xe01da194625c25LL, 0xd62018764d7473LL, 0x64643721313d24LL, 0x5a01badd970941LL, 0x481c9578781414LL,
0xa4d3faa92d1fefLL, 0xbd4b247d37862aLL, 0x5332a7ca3c2ca6LL, 0x393ee51989d5a9LL, 0x01a6e564040d37LL,
0x390c472ee27892LL, 0xf0217fe009e9b4LL, 0x5d3f04da415b35LL, 0x612ecd5b8e4eacLL, 0x757e27d2169f2dLL,
0x92853b737b7526LL, 0x9ac837c86476dfLL, 0xe956c2b45ebd5eLL, 0xd4fa6da687ac39LL, 0x60f4343669ddd3LL},

{0x64b8d778e72e78LL, 0xf86cd55efe92b8LL, 0xa9adbf2e728440LL, 0x966c8282cee1f9LL, 0xea195972b883f4LL,
0x46ac03b37e7f24LL, 0x744df253954ae5LL, 0x22e3f9a0adbc58LL, 0x6add7c7d8a2961LL, 0xba963e4912d17cLL,
0x2840ac28fcfad9LL, 0x8d8ec3ad6dfc32LL, 0xa3c788dd094910LL, 0xe65ebb61dabb5fLL, 0xb50e906b28c881LL,
0x003b11eb83e6a9LL, 0xa2fac0595b138dLL, 0x3d55a28f915330LL, 0xc343bd1849a085LL, 0x54c786629d2b42LL,
0x1d465cb22ccbc2LL, 0xd8f87fd52aded1LL, 0xecb34f46656b71LL, 0xb4cbe50f839f2cLL, 0x2df6a553cc3698LL,
0x40b2dd25f26d51LL, 0x492f3c5c6fa566LL, 0xf80dd453864548LL, 0xd4be786d8735d9LL, 0xe364511a0fb62dLL,
0x3c2df64d6d1c9fLL, 0xf640e4ef4186beLL, 0x41773025d6ff57LL, 0x6147e75d7df3f5LL, 0x49809548639d16LL,
0x01067ef6034247LL, 0x4e7c1b20deb154LL, 0x3f8172a6b98ea0LL, 0xb0691d4b575801LL, 0x136a88607a3e5bLL},

{0x0180058ca8742eLL, 0x972bc2ca1c4cb6LL, 0x7b05bbc57e63dfLL, 0x5f01049697eaa2LL, 0xc537f3121384dcLL,
0xedb1fa0b34f132LL, 0x689b1374cafe25LL, 0x802d7bca5c6674LL, 0xf8e01e75e9eb3dLL, 0xa59c2d9126d85dLL,
0xf10f603f8c4fd9LL, 0xd5a358aa84b2d5LL, 0xf8320f2a3bd078LL, 0x019bcf0dabb5c3LL, 0x43dd8dd5e173f0LL,
0x45169f788a0233LL, 0xd62daee0e9839cLL, 0x7d673cf77a53d2LL, 0x008730faf272d0LL, 0x3c08080778ae8dLL,
0x920e40fad87d7eLL, 0xbf118230ffb194LL, 0x692baf40b951b4LL, 0x83549affe4e382LL, 0x68e172f86a40b3LL,
0xaa5e2c1b74636dLL, 0xc3d7809ac68aaeLL, 0x33c344fd9bcc33LL, 0x6e6057dc7d71f0LL, 0xbceef547db57faLL,
0xec91cc1056e4b5LL, 0x8153f00c8ef4f8LL, 0xa2ca943ab03915LL, 0x079a070121782dLL, 0xd592dcec23dd3fLL,
0x44ba5fe5078279LL, 0xe6f8ed790ffa59LL, 0xe7877e834b4391LL, 0xd1ca3db32bccd7LL, 0xb382e35bff1ba1LL},

{0x96cb3b9ef8671eLL, 0x70342fff9216a5LL, 0xd635530148dcc6LL, 0xbf40909f72ba4bLL, 0xe3697761ac11f1LL,
0xf2a77a5f435c5cLL, 0xa57729bb9aaf37LL, 0x14f78a30f9bf6fLL, 0x1a7fe7f0271b01LL, 0x0b224bc83ef07bLL,
0x0d409ce2157473LL, 0xadefa793287d48LL, 0xa6b13ce8e00a7fLL, 0x74d735fd54a00bLL, 0xe2dc16285d1b5aLL,
0x8b3d55371ce703LL, 0xbb3909153586b6LL, 0x03c8c622aa53e9LL, 0x89ee3322e069aaLL, 0x325ce41fbd0175LL,
0x2cd1326421cd83LL, 0x3c47eed2daaddaLL, 0x87c2177de0c63fLL, 0x39b496d688c971LL, 0x179359349f5e0eLL,
0x3cfa9ea9345dbcLL, 0x47b1948cbfe45fLL, 0x2a13b18cf3a0d1LL, 0x00b03fc13e6cdeLL, 0x656ef26757f5d1LL,
0x7c584630c27fb2LL, 0x02f2e14ca8a67eLL, 0xfcfec527978154LL, 0x4ec09910379625LL, 0xe90fc0a898a5b7LL,
0x5beb0f3ee5d03aLL, 0x2383832708cfb7LL, 0x6905747e27453eLL, 0x1714e418f0f0a3LL, 0x53bcdef0965e8dLL},

{0x2c9b5813b90c3cLL, 0xbb9a20c8ebb80eLL, 0x045e04f3d57918LL, 0x6fe6ffb0718731LL, 0x201760abf11c27LL,
0xe289872adda7e1LL, 0x233e7ef2b2c83bLL, 0x423b4c0ba711dbLL, 0x334b15e5bd4c01LL, 0x034d1e41bff0e8LL,
0x58a436cce28ea3LL, 0xe6ef4d94b49962LL, 0xec8728db63716bLL, 0x8c8ffc95c21b06LL, 0x0beb50502d9acbLL,
0xc1eb732268091aLL, 0xe45e0c30cfed36LL, 0x31d58c384bc3e4LL, 0x8a26ae8b7a5c60LL, 0x83991e11e8a21eLL,
0xe4f193c0183e07LL, 0x691fbbf9ccb4c2LL, 0x4e5214fae905d8LL, 0x2052c969e9699dLL, 0xf6cea5a6157de3LL,
0xfd84477a6bad8eLL, 0x04f37758724bc3LL, 0xa491d0fd8f084eLL, 0x19933cec5f51f0LL, 0x93794e76e1f29bLL,
0xebd1f1c057b30cLL, 0x7ec220fa6d31d9LL, 0x867d711c9a7674LL, 0xa700cf5f177e37LL, 0xcf3fae5da3ddc4LL,
0x4e8030990c7917LL, 0x553a5ce2abaaa4LL, 0xc2296c42e2dceaLL, 0x19ae4f9b654581LL, 0x66d5fff1163703LL},

{0xbb5085e0e7d595LL, 0x12605df8a35f9fLL, 0x35c6d572c28ea5LL, 0x5099437e5f5595LL, 0xfb45cdaa8872f1LL,
0x6e012db5feedc3LL, 0x1ba0e5515be76fLL, 0xb793b687fbf1ddLL, 0x9d2c01063d4ca1LL, 0xc2e6fde5bc3a1cLL,
0xc17b11e1a33418LL, 0x436fcacef170c5LL, 0xe4c3cbc3066618LL, 0x2063665d2a1b84LL, 0xa8b5b4f2e58850LL,
0xce74bcbc892d71LL, 0xb312d96806cdc8LL, 0x82d9c95678fff1LL, 0x5d8a0120206c3cLL, 0x621f13db39bd6eLL,
0x4a5db4815f181dLL, 0x8dae6e596cebd5LL, 0x1b8b1681dd4918LL, 0x1dbcbd79f8e5ffLL, 0x135064b0968c4eLL,
0xd81e91507c1e96LL, 0xce08e072644e54LL, 0xe1648d32befadcLL, 0xd0b7f41fca118dLL, 0x7b9291b680b18aLL,
0x10ab9a2fb4f9a0LL, 0x9f462d2370dd03LL, 0xbb453f4b48b2eaLL, 0xb3c3e6d63c2559LL, 0xbe4aa3d8e8f129LL,
0x90af78e01d25c9LL, 0x2e06a8715063daLL, 0x988dbf792de669LL, 0x17eabe5b043c41LL, 0xb1f700946e4ad2LL},

{0xe329ae8a66581eLL, 0x4a5bda0ff2a313LL, 0x79577080aaac8cLL, 0x0dd34f4f929df3LL, 0x0f5e87f82b9b1fLL,
0x1ead67333c42d5LL, 0xebac8fb8797375LL, 0xdc26965e625abbLL, 0x953ce074d8c84cLL, 0x2edd54991b2104LL,
0xa45196065c2bcaLL, 0x98f56533f328bfLL, 0x8560a1a390e921LL, 0x37d2506aff3d7bLL, 0xf88576a47d273eLL,
0x562b7c9592ffdcLL, 0x2d0ff0ba59787bLL, 0x4dd89971bd39a6LL, 0x7a4a778d69a4ccLL, 0x58bad18bf5fc74LL,
0x5cac8d53dcc72cLL, 0xba7e9c7a2b57d7LL, 0xff544acc98f08fLL, 0x1d22f503712081LL, 0xcf868290f04defLL,
0xba48ab7c61a8abLL, 0x3ca439f055f713LL, 0x2401e3a43338e0LL, 0xb7c4b19cf1edc8LL, 0x37db6b0d8991a7LL,
0x10ede95c9c35e6LL, 0xa8f021fc870126LL, 0x6e5909a7f3217bLL, 0x33772e647266ffLL, 0xa5c8fd0c786e0fLL,
0x04f0bb34025c67LL, 0xcc33c6a49bf101LL, 0x45c563f33f807dLL, 0x6e95e9c2b5e349LL, 0x3a0e55d42d44b7LL},

{0x611138d0e928ddLL, 0x24d7958e8e6149LL, 0xc66faf12b50f45LL, 0xeaa5eb19337961LL, 0xe68c81cb35d5d3LL,
0xed1fe1f1b8d443LL, 0x612ca593de8afeLL, 0x6c15ee22ffb8b0LL, 0xc27152ca5a1e77LL, 0x0133b8165e3ed1LL,
0x608c9c1a6ca4aaLL, 0xdf5272bd1b6425LL, 0x6f7efc5b2bbfa0LL, 0xb49b5f0c67ee30LL, 0xf4ef0e7ed820cbLL,
0x4b14d077b672ceLL, 0x3a60f2386c0218LL, 0x9e8d6e5f6caddfLL, 0xa53ccecbae8684LL, 0xd3183beeba0cefLL,
0x4cd21e6afc08e8LL, 0x5db41995d15a93LL, 0x6afe570246af77LL, 0xd0994bc305b27fLL, 0x2de99a0885c909LL,
0x1629a47aaa161fLL, 0x0f6b6d45ff8967LL, 0xcfc4e83f5b469cLL, 0xcc22586cab3936LL, 0x29e6b3f94d122eLL,
0x83f00e419d8980LL, 0xbb282b6f3efdefLL, 0x30d80463fb25e0LL, 0x1846f8f1b935d3LL, 0x3c03ed5243b7b4LL,
0xcb6b0e6e4c770fLL, 0x8bc2856390163aLL, 0x73a332bc2ebabdLL, 0xb3aeafedbc8c08LL, 0x74ff7726398cd4LL},

{0x0071d5d3644b97LL, 0x45dd1ae0369e9aLL, 0xc1f518cd384512LL, 0xb933bc25cb3402LL, 0x9377c50007d647LL,
0xe609eb009c9245LL, 0x7d99fff828ba6eLL, 0x9f0adcca6cd0a9LL, 0x5c5cf8366b699fLL, 0xf00f513ad9e29dLL,
0x7c2ecfdb5afe40LL, 0x1f131691f0677bLL, 0x30e1df0cce8710LL, 0xf3c52df030e941LL, 0xb2bb6b650cf2d7LL,
0x012a5a2d11f1b4LL, 0x4699b78e898918LL, 0x977b2e06972b36LL, 0x674e2619e6be97LL, 0x93007948f99eeeLL,
0xaf2b5b80b81bb3LL, 0x417446ac93bc16LL, 0x14fb20c6ab0e24LL, 0x3ffc77d1672771LL, 0x36580afea2edecLL,
0x48942ed95911c5LL, 0xfa312a7aca8f83LL, 0x992e36a47ef1dbLL, 0x3937ff39b1a9b5LL, 0x2af79ef5c48c64LL,
0x6c88d58111a0b7LL, 0xb6fa6dc5f7c8dcLL, 0xb1acc64f2b083dLL, 0x332baac65b4febLL, 0xe58dae530ad4afLL,
0x0fbdb072d0ba36LL, 0xe2607b065b6fe4LL, 0xf803ae22cb2a6cLL, 0x9b639dd91166ccLL, 0xf5e430b9cece8cLL},

{0x687c1dc2ac5898LL, 0xb429122b168f1cLL, 0x4248f91ae51605LL, 0x1c24d7f1578ba6LL, 0x1dec5a6c003598LL,
0xe3c04b01a812a7LL, 0x2df7909352ceceLL, 0xde31efaffdd0d4LL, 0xe4a7f11873ec87LL, 0x4768f7b8d77583LL,
0x23b6f7bae4521fLL, 0x8fbf571e568d5aLL, 0x577ad8b71f3721LL, 0x718b68ac1ada36LL, 0xe10689cc83ea91LL,
0x43f73798b295f7LL, 0x6e2b078c8d68e5LL, 0x613c3bb265ca36LL, 0xd25d07032b8c80LL, 0x843fe3783b5959LL,
0xe918f7789f0d33LL, 0xafac1cb1534684LL, 0x0fb3c6c442a94bLL, 0x167f58645b56c2LL, 0x76132472470129LL,
0x590ae9be533d39LL, 0x75adfeba5e6230LL, 0x30dea290d933d7LL, 0x08cc4d30a4af39LL, 0x09bc69be193a2fLL,
0xf7f8ff9f03af3bLL, 0x3ad1a453e9dde4LL, 0xa534709b6e15c7LL, 0xc6ce7d4efd42e9LL, 0x5e947977595b68LL,
0xca674d0c7541e9LL, 0x97f178a43b6057LL, 0x137a6483c7653aLL, 0x49f1eec3082cc7LL, 0x70824eb5bebf04LL},

{0xcf95519563f7c7LL, 0xcef140efdaa431LL, 0x4f8ddc5fb70009LL, 0x27710736a485cdLL, 0x41b05dfead9e7aLL,
0xdcbf8e83a3a89aLL, 0x23e46b5a421a08LL, 0x84f0fb922099a4LL, 0x120b226eedd549LL, 0xcf4706582b36f4LL,
0xe3b718cabb9c11LL, 0x03db1daab9520aLL, 0x3a29a8c65c45f6LL, 0x0219e82dbeb36bLL, 0xb351c498a8dda8LL,
0x0ba2a5607f3bf6LL, 0x0b95be14721f63LL, 0x62d3b4d2b1fc16LL, 0xf46a95de23a55eLL, 0xb70c2f136e83ebLL,
0xa0b215f5837e73LL, 0xd76368870bd5bcLL, 0x0372cf15e7ff03LL, 0xc992d958598014LL, 0x1fb03e9712f2c4LL,
0xa73b9107699fb2LL, 0x239ad1d706b5f3LL, 0x3623dab66fefc1LL, 0x8b5e04ac40e7edLL, 0x77eaadd7c4d35cLL,
0xb3ba11dde839a2LL, 0x621e7ab334235dLL, 0x29f2ed9f1990d9LL, 0xe0d731952272a4LL, 0xd31f58d8cfad64LL,
0x57690ff74579feLL, 0xe78fb0fe43c6cfLL, 0xb127e3c5c7da88LL, 0x1765c8883fcd01LL, 0xdc0028f618172dLL},

{0x07d8f79c0e5b79LL, 0xbdff41e18ee3b3LL, 0x0990bd1c710888LL, 0xb0ef52eb6da5bdLL, 0xb790ff7419e17dLL,
0x22ab4221d42b9aLL, 0x35bec4ded01a53LL, 0x6a2f35fd63b686LL, 0xdb66f3c21b9291LL, 0x165a5fd321d034LL,
0xf2ea034bd3a6b6LL, 0x4d47388e2680b7LL, 0x018dd250cfd53bLL, 0x53babaed27080aLL, 0x73c54d98e4a365LL,
0x6a77f2e71cfab6LL, 0x4f9539f7e67a64LL, 0xc35beaa6ab5528LL, 0x1698a8ee44d10dLL, 0x01e623ff7096e8LL,
0x96a68072d59c56LL, 0x6baba4b0d232eeLL, 0x725a1f9e0fbeb1LL, 0x97728ef73b9a8eLL, 0x16ecfe23a3bdb6LL,
0xf035aac743b427LL, 0x202c094281f68bLL, 0x1c8be9e39e4591LL, 0x0959fad0920ae6LL, 0x15a97f475dc632LL,
0xa3fc9e9363688aLL, 0x89cea147f0339bLL, 0xd1ffe6e68570d2LL, 0x329a0b16c32fa2LL, 0xcbd5818383dd8fLL,
0xc26f57abe7c8cdLL, 0x4d680e55e8a77dLL, 0xfeefbd47b284a3LL, 0x41bc9077e7df69LL, 0x1c32ea11a0df3cLL},

{0x2ea8501eab0c69LL, 0x63dff30ea51c9fLL, 0x8de69a045d957bLL, 0x4036f90d8e90b7LL, 0x5886f2e5059e5dLL,
0x7341e707011ecaLL, 0x8d6006677dabf1LL, 0x2c6f2040741941LL, 0x5058a43d3958d2LL, 0x29eee2b01178b8LL,
0xeb9e382e6ea2e5LL, 0x62e44ce8f6b19eLL, 0xa5b4444f78d77dLL, 0xc12755f1de34c7LL, 0x8fd001eb8d0d91LL,
0x8a3ece83c541b5LL, 0x659f736aca9076LL, 0x1c1864cc5b30f1LL, 0x1b9f901459a142LL, 0xf5571fc19f94a3LL,
0x39e842e17176caLL, 0xed2a1659a97f8eLL, 0x625e74d131b3daLL, 0xbdbdfeaa0366bdLL, 0x95ebf86c33a687LL,
0x4a09faea206cd1LL, 0x29f59174377238LL, 0x908e6c956350cbLL, 0x686a225a26548cLL, 0xa45140d1ed5b76LL,
0x75e9ea2087732cLL, 0x14dd568be007bfLL, 0x3668e3791bdd4bLL, 0x56f9aa39df5785LL, 0xe7b37c964271c9LL,
0xc5211e837c726eLL, 0x374513cd4cd34fLL, 0xa5c71ff1a4195eLL, 0x4e234c5adc13b4LL, 0x75093fc66c8fafLL},

{0x2ec02dd6ea2715LL, 0xd8676bb21e7f0bLL, 0xb4c22ceadbd907LL, 0x9ccaf78857ea36LL, 0xa28da605bbf2d8LL,
0x723651fb07c86aLL, 0x07039b49d2fa32LL, 0x40dbb6dc2ef93dLL, 0xda48f7e9d5eb92LL, 0x45bc6190b3a9e4LL,
0xfc84b55352b994LL, 0x25f44b36a3fb83LL, 0xd09a8f4ab7d78eLL, 0x0829201a523b21LL, 0x966e0098395656LL,
0x5984c4e317d930LL, 0x581dd2ab677c99LL, 0xa92a70424c5aaeLL, 0x4ea1dbaca67de1LL, 0xe45918a0d6d560LL,
0x1e5c75efdd907fLL, 0x99a6e56cbb015fLL, 0x04fd11c8ae4d05LL, 0x83a72f3e967bb6LL, 0x2ddf23b892d1e5LL,
0xd648bbe9e5f8d3LL, 0xd4b128d667ff6aLL, 0x781dcd435b03f4LL, 0x1a1cb99fc298e1LL, 0x69d80c51941a26LL,
0x5263476c788bb7LL, 0xdb0b584b59ec8dLL, 0xd95a4e9a6a95c5LL, 0x5263b0eb0cc8d4LL, 0x98e62e5116ab09LL,
0x97564c79d4b733LL, 0x39d708c3284fb2LL, 0xd2cd596efe674aLL, 0xa9e3b1f33b4473LL, 0x70b30aa67c0c2cLL},

{0x3532c9874c8ce5LL, 0x680a796f9db4b3LL, 0x64e5825663090fLL, 0xeb0a67604f3f9bLL, 0x7c4716c88afa20LL,
0xcecf4b6b1467f8LL, 0x342600406fe556LL, 0x200290eea56903LL, 0x36562b6cff764cLL, 0xb02d3847d68f8fLL,
0xa26c2ab20fe063LL, 0x5de36be096db8dLL, 0xac5998b94e3c17LL, 0x4c8808ebb9bf53LL, 0x4bbf0a436470daLL,
0xd3875253f7b0a9LL, 0xa99369bfede348LL, 0x8c3391fd3a5f95LL, 0x5005f88c89d735LL, 0xacd8196d21d41bLL,
0x5ba2ce34f48817LL, 0xda3e7f4332994fLL, 0x8cfe88c8ae18afLL, 0xe4df8b64d16e61LL, 0xb0f200ab8229f9LL,
0x5a15b4ad681a60LL, 0x350a1bb85a5708LL, 0xf5731809fe17daLL, 0x9da29858778783LL, 0xe496533ffbda6cLL,
0xa590c76b953dffLL, 0xedbf61ba227191LL, 0xf7fd713fd0b4bfLL, 0x4a5e6df9905845LL, 0x42ed273f1fee88LL,
0xe56d34cbb2866dLL, 0xcc76209f9773ecLL, 0x4c21238f991ec6LL, 0x7adff263cb22b1LL, 0x4fb41d94f97f42LL},

{0xf26d90e0b24a1aLL, 0x37fe90421cee92LL, 0x5cd69e29e95550LL, 0xbec2bff0431bc0LL, 0x6acc812fa97ad4LL,
0x4f19e44dd33a0eLL, 0xd9280b1ae70cffLL, 0x6575a036db7f1eLL, 0x7bf2ed31bcef8eLL, 0x45dfb49b8dc51dLL,
0xe1fd10fb1b59b8LL, 0x092da05f342c0aLL, 0x01fa56a0375319LL, 0xc1f5ad03dc627aLL, 0xcb1f2c96f11444LL,
0x5d67a093467a43LL, 0xa832f56266f0bfLL, 0x7a464d7fab7c48LL, 0x42561af703a045LL, 0xc1c9b270211af3LL,
0xedcaf802cfd336LL, 0x6f9ba5cc39c3dcLL, 0x585554fa4224caLL, 0x4a7216b8d2dd3eLL, 0x16c2d8b31e6fa9LL,
0xe9ae301e1bfa98LL, 0xac8389842b368aLL, 0x158c5060209885LL, 0xc01a2c3f5b7bcaLL, 0xd20124920faa1cLL,
0xa2217820d1fa40LL, 0x803272c88d1844LL, 0xc2554237c9ecc8LL, 0xd25f509a6db1deLL, 0x325148c1726f18LL,
0x398c66b1339048LL, 0x8c8c43dd7f2c26LL, 0x24cf4ec93ee498LL, 0x54618829620375LL, 0xeb494db615a50fLL},

{0x69e1fb949b4215LL, 0x3e02e353426513LL, 0xbf6ea2adefddedLL, 0xfbbb781d40e52cLL, 0xd6ebec825d94a0LL,
0x3f84de44b6fd50LL, 0x0b466ea0458290LL, 0x3a77f7804e0c62LL, 0xb0ce750e2b2078LL, 0x69f346f188a43aLL,
0x24ef26f7c284a3LL, 0x544ea716d5498bLL, 0x3e1f23b1154dcfLL, 0x6d5c580dbec7f0LL, 0x120302c7a16ee0LL,
0xbae4ae638ee502LL, 0x60cd112182bd84LL, 0xdbc443744789a8LL, 0x7faefcebed3a2cLL, 0x579c0f77cfa536LL,
0x0d920b050cb068LL, 0xfb2fc616ee5eb8LL, 0x3b7082e645d419LL, 0x40df3b620a8474LL, 0xdf360190d74ec7LL,
0x28f0d33396ee1eLL, 0x3c007bfb335325LL, 0xac5c5327fcfbe9LL, 0x9daecd75584e11LL, 0x770aecaa7200f5LL,
0xef955be6081878LL, 0x8c906f9fbbd9a8LL, 0xf16d11b5a2980cLL, 0xf837a8f49c0378LL, 0x33efbbae308e71LL,
0x0bda652822a309LL, 0x8990e49a4320ceLL, 0x8bf60c5517e853LL, 0x0b0f2a3d47d09bLL, 0xb07d28e7903ac9LL},

{0x5009b61262ab9cLL, 0x0161bb90668bf4LL, 0xa314e46c502058LL, 0x447250d9698fedLL, 0xc3e4ceaa255d41LL,
0x5ba4045c2fdba8LL, 0x17b0720f52e736LL, 0x0eb0036d8439d0LL, 0x9e15116b8245e2LL, 0x3dad88738ceab0LL,
0x260986d154e9a9LL, 0x56cd13e67e508eLL, 0x9895906f7a2bc2LL, 0x4970647a63ed02LL, 0x5e192810f2e040LL,
0x02e7f4cad9b4cbLL, 0x18d5850dc181a2LL, 0x05204ea9653f18LL, 0x2d3b188124823dLL, 0xf9b34ca3d2c93bLL,
0x2e5ba515010f68LL, 0x7308114d65f874LL, 0xacbf4d6286131dLL, 0x46681d439816a8LL, 0x15fc07b05c47feLL,
0xf0ef6a332c3132LL, 0xc4630529dd2021LL, 0xa743a1e9423e63LL, 0xb12af7fe3d806eLL, 0x0cb7d03c2afdaeLL,
0x7abe068af28323LL, 0xfe75b567a2c0c7LL, 0x069313cf6c1f44LL, 0xa39aeec0ddcc87LL, 0x747c3bd20c1471LL,
0x876af6b8558b0fLL, 0xeb0b357c5d8f97LL, 0xc64ac9dcac22f2LL, 0x856e4341b42b50LL, 0x663b16ec5eb01fLL},

{0x0d31dd990e70ccLL, 0xf7203530ab3d19LL, 0x6d42eb5412ec69LL, 0xdc9e4fcaf97880LL, 0xe0dcd2d94a10fcLL,
0xb5f39a9e831217LL, 0x4b084adf9c02c4LL, 0xd3cabf53a97846LL, 0x4c331980146846LL, 0x3c9f7c840833beLL,
0xb0cb542c3108b1LL, 0x9dcf7401e6f79aLL, 0xc1f27ed5dd4e0eLL, 0x509cf69e83c56cLL, 0x15ca00d43e1758LL,
0x5948602f5bf14dLL, 0x1d129ae6b9f4eeLL, 0x2b58f973ae2956LL, 0x6a6c792feb0c13LL, 0x62474058c00758LL,
0xcaab48f22b2e6aLL, 0xed88328618842dLL, 0x0418ebd349eb34LL, 0x846eda10087342LL, 0xe8b6c21b95cbf9LL,
0xcc90523ed0cb59LL, 0x4c9374718e79caLL, 0x60c8fa29dd489aLL, 0x41f2190a03e88fLL, 0x8ac12bebb17c5eLL,
0x3195835960d662LL, 0x2317a3d2d90eadLL, 0x5f5aeb6d34f4e1LL, 0x7a39957a01179fLL, 0x3f88d79fc83f9cLL,
0xedb1049a771b1eLL, 0x30a85067c640edLL, 0x06cac8047923deLL, 0x59bdda0f1b1b9dLL, 0x7a014eaecf61b7LL},

{0x292e8b0f865638LL, 0x4dc1de3d7f5ddaLL, 0xd9b1b7557b4db8LL, 0x54813ab90c75a3LL, 0x9b35f03246f1e4LL,
0x20f760465bc347LL, 0x0da41ba5991181LL, 0xa6a49de8fdf505LL, 0x60b1ea116f81a6LL, 0xce2716aa9919a7LL,
0xe3fce68f208dd3LL, 0x05d5b9594f643dLL, 0xded74364c812dbLL, 0x16b6e7e4269696LL, 0xad975ff975a727LL,
0x4d6e503b6ae9a9LL, 0x9ce664850ed1daLL, 0xa714650763250bLL, 0x944b7b251c3e6bLL, 0x0d37d4e4854c4cLL,
0x06c7e1c3d4b917LL, 0x5602bc69558908LL, 0x92f5ddd9a20bbdLL, 0x84d12a16b5963dLL, 0xd1426dd7f44f09LL,
0x06cca7d8cd71baLL, 0x710072c1b4ea7aLL, 0xebabe1e8242f72LL, 0x69960c6c0d5bf3LL, 0x2084edb90ada1dLL,
0x235ed7d8a9fe39LL, 0x3b133ed8a3fec1LL, 0x132c4509579af1LL, 0x203ca5447787a5LL, 0xca938128fcd756LL,
0xca569d31b6f05dLL, 0xedec4129270543LL, 0xff17078079c2aaLL, 0xf642caa8568a3bLL, 0x8d1f6c3bf9b5e7LL},

{0xc947c61701ce12LL, 0x1a3808b18cb73cLL, 0xd1d7543be23892LL, 0x9917eefd8b4b7dLL, 0x0eabef30f24b08LL,
0xb72c10d49c60a3LL, 0xc01344f22cc2a2LL, 0xb97c57f2a37b00LL, 0xf82a2f9338e520LL, 0x5a8b9c9ce0dc1dLL,
0x8a4d7e7260e257LL, 0x62046c5551c0e9LL, 0x19811c1011cf28LL, 0xdc158db4a957c2LL, 0xb516e794206aa4LL,
0x4a9e535622d8dfLL, 0xbea44b252b2ab8LL, 0x7284568528acd5LL, 0x239ab1d64c7025LL, 0xbad538907922d6LL,
0x57fb163fcb9ecaLL, 0xad97c1507e480aLL, 0x78e8cfc81ca935LL, 0x14eee2413bbe9dLL, 0xe349073d92ab5cLL,
0x8ed191d530d9afLL, 0xf3a72b6e194e41LL, 0xd26925b22eb6c5LL, 0xf709c6088bb419LL, 0x8527923aa6f4f3LL,
0x1345fcb8916f88LL, 0x9f82d7a298174bLL, 0xb0a41e5d16d9d4LL, 0x28c7eab4098fe2LL, 0xf34abb591392d4LL,
0xa5084515586118LL, 0x71f3fcfcee19ffLL, 0x180d1b40c23b7cLL, 0xc18c22be085cc3LL, 0xedeb86d04f3c78LL},

{0xc56c61899b8011LL, 0x2cf78b1bcd5b77LL, 0x99247be60f0cdbLL, 0x4c8a9aa7a58409LL, 0xe2bf0ad4cfe9b6LL,
0xf79b501f91d364LL, 0x5fd2c40e48e881LL, 0xc650973fb8e681LL, 0x7c8ae6d3aca02cLL, 0x7a01c329e3bf17LL,
0x3b126f2cda1e76LL, 0x229d405bbc41d3LL, 0x5e028a9f388566LL, 0x97e13e1dfee5e2LL, 0xaa02da00a5271aLL,
0xbe2abd92296fc2LL, 0xe380153ffa1a5dLL, 0xdc3c184ca2fd9aLL, 0x8dd7381eccc7e0LL, 0x55a7fef2252572LL,
0x76da25ac98ef00LL, 0x3e12a21d43ef92LL, 0x28c5f1d9e71a96LL, 0xb7cd9a47a9c9d8LL, 0xaaf77a03539742LL,
0x9f8854a9983a9cLL, 0x2bcde940d64350LL, 0x6986616ba3f75fLL, 0xe80cc522c68b65LL, 0xf03f78b91d9f6cLL,
0xfdf9170e4ac9f1LL, 0xc84c3819797defLL, 0x03bbfca0340880LL, 0x2893d145bda408LL, 0xdf07456e5388bfLL,
0xcbadcc8ac22dc9LL, 0x365807ab820d70LL, 0x29da8be4c0de87LL, 0x756ee3a7865bfeLL, 0x46439df366b70bLL},

{0xab960b51e728dbLL, 0xe2e3c346921e4eLL, 0x74c6317baa49fbLL, 0xb3efd421fb40bdLL, 0x979d2df24bca93LL,
0x98d5bd5de71195LL, 0xbc030746a50c59LL, 0x02cf2a4b1b9812LL, 0x467af79145cfaaLL, 0x0ed643c7b530e4LL,
0x181ef7d406026aLL, 0xf6ea606e325377LL, 0xa302d06af1f7ccLL, 0xc7658f6ae6defaLL, 0xbbe5314d959e1fLL,
0xbb5757386c8799LL, 0x8759670183f618LL, 0x58e0cc3816f883LL, 0xc113183a0578eeLL, 0xce5456e86ee96bLL,
0xc04285b8c56bb1LL, 0x74e5fb66d586e6LL, 0x9d8eea215e70c8LL, 0xf4a00feb7bc2f6LL, 0x369c2bf470063dLL,
0x5b267be08f0594LL, 0xc26fb2440b1ac6LL, 0x8610ef5a140769LL, 0xbb3d5b50a536d4LL, 0xdf6c30bc09f971LL,
0x74e572ca84d171LL, 0x2deb91e812d860LL, 0xb17ac9ae5be211LL, 0xc95a0e3f542c78LL, 0x46397245b13a99LL,
0x1806ab9ceb6646LL, 0x1b4161b0ec2edcLL, 0xfc536e2a24abf8LL, 0x9f7207bf519f1bLL, 0xabf95b0d0d3caeLL},

{0xd9e17ba1bf7678LL, 0x6526f524fad677LL, 0xec243ad271d0cbLL, 0x9b1c06cf737605LL, 0x0a36697c74beaaLL,
0xfa0f0056a6bd7eLL, 0x9f2d03db497a93LL, 0x027d76e6e8692aLL, 0x72ceb29c5913a7LL, 0x55eb297dae3330LL,
0xeb676e7345fb39LL, 0x7021192efd5b47LL, 0x462906905e7511LL, 0xe005f52fd8da5eLL, 0x1288c01960d735LL,
0x3460b18eafd2d8LL, 0xfaa9b1c3caf426LL, 0x5035e585d9fd2dLL, 0x85636dff1d4e42LL, 0x600c4b7f664267LL,
0x02b21e6a8c7a03LL, 0x79ce25c264e2f4LL, 0x035a7f32c227eaLL, 0xbf8f711445a7d7LL, 0xd0b5e3b336f71eLL,
0xc454a416321483LL, 0xbdc1a7a9d20deaLL, 0xe1ee4744e83143LL, 0x5b6969f2864529LL, 0x17a6b42d6346abLL,
0xff0fb6edf2265aLL, 0xba75b0991f6dfbLL, 0x6638c1d7243ff8LL, 0xe7806af6600486LL, 0xdfe3bc58f31717LL,
0xb0c4adc2717922LL, 0xc11abff0b4a290LL, 0x43598e076f60beLL, 0x2ef17ad2f77605LL, 0x3a41a09d974da6LL},

{0xee787846e7ff26LL, 0xce05d869fecf18LL, 0xfde916d95f9357LL, 0x4c1b4dd723b90dLL, 0xb1f024400d61a2LL,
0xf51dfebc71b770LL, 0x461e7f725d9637LL, 0x2b1587ff40035eLL, 0xa2cafbcd0c6b17LL, 0x2e9efaf6986045LL,
0x80e339a823ea56LL, 0xfebfaa02609bb2LL, 0xa33955624e1602LL, 0xa137b84639ef0cLL, 0x6e2ecf420a6d6bLL,
0x69f13acbea8f97LL, 0xb4d36c41e3a867LL, 0x1352aee4798c08LL, 0xe3ec254ddf35cbLL, 0xab600d90f13919LL,
0xd00cc1d401fd2cLL, 0x1c629e621756d2LL, 0x090f8d6e0895faLL, 0x701bd1b0a355ceLL, 0xa53c7c91b15eb4LL,
0xdd8579d4dd92e4LL, 0x03d1c960c63d55LL, 0x215a8fbc09cc85LL, 0x59c1e6069b6dd1LL, 0xa0428bfb223ccaLL,
0x46d131153e9982LL, 0xc5dab0c9ffb93dLL, 0x682db866d6503cLL, 0xa481c48384a087LL, 0xa417c564567258LL,
0x2ec7b9722b5c5fLL, 0x2d491f9cf79086LL, 0x30cd268b1088d0LL, 0xf02e69b1441963LL, 0xd9841b5339d18fLL},

{0xa26deb7b957527LL, 0x337f3bd67d3c51LL, 0xe6839a4d5fe4cfLL, 0x1619c18889be68LL, 0xd971f0f57d1016LL,
0x56213ebf152a2dLL, 0x9c0e0394832c92LL, 0x9e6fc90ca28ba5LL, 0x9c5151dfbb8394LL, 0xf49fe4cf2a3f7aLL,
0x97f4db054b2b34LL, 0x2a4c21abf6406fLL, 0xeb941a80bee3f8LL, 0x7615468e80e77aLL, 0x0f935ebe8e8842LL,
0x959f2b3ba1f50cLL, 0xd6bc8b614e39deLL, 0x3c43d13746983aLL, 0x7956e617131247LL, 0x56de3547cf1010LL,
0xc16d5d1fce2bccLL, 0x3e73e5ef9fd691LL, 0x1211c1a27803adLL, 0xf1c9644aac4ba2LL, 0x8d67134e3be189LL,
0xd8aee617c607cbLL, 0xf62677b30d8eceLL, 0xe7df69402b2291LL, 0x6ec102f220e09fLL, 0xa6223e874c3e53LL,
0xfb474983ebdb9fLL, 0x806832bde2f4e6LL, 0x7c25ef688134b3LL, 0x4aadca3409a6b3LL, 0xbdccd638f3b19aLL,
0x2b01f18625fdd1LL, 0x0f5e91c28af081LL, 0xf28e4dcd9077cdLL, 0x9229d87caebbfbLL, 0x072b846b4d2ce0LL},

{0xfd7a25e195d67aLL, 0xec9546899268d2LL, 0xff3068a2e9d0c2LL, 0xaf9f2fc2de9978LL, 0x01b47566d0faefLL,
0xfc5a8eab966720LL, 0x4b981c9fe7ff10LL, 0xb4a0aa0873484eLL, 0x25a8b544ed8801LL, 0xc72530c2e5d37fLL,
0x94b0483e74e4faLL, 0xbc5ac97d82cd68LL, 0x1a23d34cce0d52LL, 0x6e4d17a8475b19LL, 0x63493b14551149LL,
0xd36db24ae5ced3LL, 0xa24a53ea6aada0LL, 0x230cbe502aa32eLL, 0x2aa07485f281e6LL, 0x66777be0d719c7LL,
0x5d3ab65be78916LL, 0x06076f42e71bd2LL, 0x273e56dd3eccfdLL, 0x5ef1c9394b6a9eLL, 0x42f1f49590ffa2LL,
0x1e6ab6994e56a8LL, 0x8d54a339e301c2LL, 0xefff698c46e74eLL, 0x6721df7c5334d0LL, 0xf36cf6a93bf3ffLL,
0x91d8979d8fe631LL, 0x321dc8b5ecccebLL, 0xa237eb1423c395LL, 0x4623a16cc50b79LL, 0x83f616f60d8114LL,
0x32c15a65536b82LL, 0xe4a00d384d99cdLL, 0x369206bbef6fb4LL, 0x42a720e294a9e4LL, 0x768a1c77e94dacLL},

{0x31d4798dffbd75LL, 0xda46bbd77e908bLL, 0x0fc027a69fccccLL, 0x4204ee745159f5LL, 0xc14155873d42b2LL,
0x7ce0c031527eaeLL, 0x22fb1c9d6da9a7LL, 0x33c940531510c9LL, 0xd938e52464ce71LL, 0x385b73fa95a2d3LL,
0x597bf6362dd268LL, 0xf9901921654409LL, 0x7c8d064cd5b4eaLL, 0x80e8fe2f1b3288LL, 0xee188609ef2cdfLL,
0xbeee34a1c48fe2LL, 0x459cefca35857fLL, 0x33b5320fbbca79LL, 0x7789297027b6a4LL, 0xf1debe5a09d013LL,
0xfd5d818a56bf63LL, 0x19a092fb1ec45cLL, 0x526f5d3ff64331LL, 0x9b8295291aef56LL, 0xd6963c3a92c34cLL,
0x065482a033fbbbLL, 0x9b9ab43410d764LL, 0x44ebbd99c86a86LL, 0x4c087234311b85LL, 0xdb6e5a803ba13fLL,
0x760c159ce2a619LL, 0xd58d83243fe0baLL, 0x1c1e5e83aa79b5LL, 0x75c8bbae9baabfLL, 0x2ed91bdb632ae8LL,
0x8e46b443cfffe5LL, 0xafa4f53f148577LL, 0x0be538701c4afaLL, 0x3fd89c44ef7cedLL, 0x060dcdc21e9368LL},

{0x0a5b9e2ba4a53bLL, 0x63db0419a96d30LL, 0xf68e038377a61eLL, 0xaa15b78389ec74LL, 0x5f532809ff80f1LL,
0xf2892acc49ca4eLL, 0xe2d68174c08e81LL, 0x378254a38f5138LL, 0x0b060222bb20b1LL, 0x8013f6f4745c47LL,
0xef08e87e2c197dLL, 0xb69d5ddaadf417LL, 0x7bc66fe482b730LL, 0xf4bd76d3bb1dfcLL, 0x09492b63f2935bLL,
0x43875dd685ab00LL, 0xe15a528f666486LL, 0xaad1fac042ef90LL, 0xb3bb7b8ef9c2e9LL, 0xc78967b9392a1cLL,
0x2f05fd5ca0230bLL, 0x1008345afdd18cLL, 0xdac73c313ace60LL, 0x346d535e500b62LL, 0x12e6357496459bLL,
0xecccdac5a34926LL, 0x3b880f7098608fLL, 0xe66c3352a9cdd6LL, 0x049b176ff1a04dLL, 0x897d9569948066LL,
0x806230e9740d6eLL, 0x179ebf2b7952abLL, 0x3a2c5079b5bbeaLL, 0x73261b85c35fb1LL, 0x5b917dc1bf7e57LL,
0x9c55b95581c1d5LL, 0xe1fb86e6219639LL, 0xc72a22d8404448LL, 0x95dc5d7b966027LL, 0x457f3fec730d5dLL},

{0x469cd82a2b3cecLL, 0x021d9de560b8d6LL, 0x85b4d126933886LL, 0xc8265dafb87325LL, 0x8741d99af7f420LL,
0x1329e52d3e66f8LL, 0x7d37c458a6ad05LL, 0x1241c5ccbc355bLL, 0x7fdcad3c3c269dLL, 0x05f04a0a23acc7LL,
0xce076aba97cc18LL, 0x74b36afc4645b2LL, 0xcd7adaad8f5b91LL, 0xbd0651649dc722LL, 0x3d9b6437c667fbLL,
0x6827ec09eed45eLL, 0x8ad6102faa934fLL, 0x1a80658c0bfe1eLL, 0xef1749235ab59fLL, 0x75478ad5949a06LL,
0x49ce6e19841851LL, 0x8df41dc39cc628LL, 0xbd9dc9bec89c8bLL, 0x7771a21fe8028cLL, 0x65082929b051f3LL,
0xc657798a3aebd1LL, 0xce9c37c494cfc8LL, 0x156efce8330e42LL, 0xd0d95860d39040LL, 0xdfb0fb66f814c4LL,
0x4b421540a0aeceLL, 0x9e767cbf7e9c49LL, 0xeeea5b5c866a9eLL, 0xe2026ca4bfb067LL, 0xec9fff1a5d41c0LL,
0x290ed4da32d333LL, 0x65208d00dba471LL, 0x99e1d15a83b736LL, 0x5585401976a265LL, 0x1668daeb4aad72LL},

{0xcdabffa646bacaLL, 0x7a6c7bb29875f7LL, 0x1a87a53a2139c1LL, 0x1ecf7ae823158aLL, 0xb1087bf595f7f2LL,
0xd8e668fe076e4eLL, 0xb0794cf137a863LL, 0x81e2a419a320acLL, 0x8090b1d39e5171LL, 0x813903d5f1a68cLL,
0xba31f8211e56bbLL, 0x52413eefa30a66LL, 0xaf88d053eb8a4aLL, 0x4d2235fdb9bf91LL, 0x69a947973d3ed3LL,
0x0dbcf583c26ffdLL, 0xb6bfbee58458b7LL, 0x5a12f3e625e5d8LL, 0x53ae22c2b1bc6aLL, 0x8e6f7e7bbde691LL,
0x2b692a6746d3beLL, 0x686438e4ac66e4LL, 0xaa77be21178471LL, 0x36aed3fc3ff079LL, 0x8c9373ebb1c2f7LL,
0xbb99fd5bfaebc1LL, 0x665902d08610e8LL, 0x8af0db91b80b6eLL, 0x00d4995f9a6ce8LL, 0x1d503d26442bd9LL,
0xe61b181dfd0949LL, 0x2a02f5075a277eLL, 0x76caa7ddc435feLL, 0x5cedae4cae7a57LL, 0x4b5f7c58c4f214LL,
0x1446a05c1023abLL, 0x459c93a9f5ad37LL, 0x1ea5f5aa4060b2LL, 0xa48215350fcd60LL, 0xac21e2a4729a51LL},

{0xf18cf87ecf430fLL, 0x0fd48c241ac6bcLL, 0x33964cd2e3d9ecLL, 0x8bebeffc2e848eLL, 0xbb84a5c2ea9b0eLL,
0x311f20ff7ce601LL, 0x752573f4effe98LL, 0x79dbe4d184dce5LL, 0x6bb04def99d322LL, 0x7d2d5f23c2475bLL,
0x7f93f7dbddf04eLL, 0xfff303e751fffeLL, 0x08d205f8999a3dLL, 0x5750d14f75e056LL, 0x1107a3f96ca8e9LL,
0xf62b50c8ad9f20LL, 0x53e7fcc55d72d8LL, 0x6345dec1054cb6LL, 0x01d52c21dc654aLL, 0xaa0bd78e39c594LL,
0x2265b675381cd5LL, 0x57cc03dd65f821LL, 0x1fa373049059a8LL, 0x9885886b48085fLL, 0x8357ab98192a14LL,
0x5bb6cfbf84048bLL, 0x5cf862f25ff6abLL, 0xc9382e36ab2dbdLL, 0x2357b5ade91fcfLL, 0x2db77558ceef24LL,
0xd4a0cb3ba50a2aLL, 0x12c3cb633dfe47LL, 0xdb805410168807LL, 0xa5e635ac766e1aLL, 0x25252810f49fadLL,
0xcae296fce18ed4LL, 0xb9932d5822c519LL, 0x4b7006cc54ea84LL, 0x2546d761d284cfLL, 0x2346d0a11b1ed9LL},

{0x81ce0d028c4474LL, 0xc8002fd0315372LL, 0x8670db1a6ad6ebLL, 0x4c7f942260e9c9LL, 0x822bb2c423cc53LL,
0xe3b67febea3672LL, 0x59c24223d913c3LL, 0x6f4b196f69400fLL, 0x51bfb6cc7f3603LL, 0xfb9fbef84ffaf4LL,
0x7c1632636806f6LL, 0xa50ec42076931fLL, 0xf68b2be9e5e7adLL, 0x7603302a518bd4LL, 0xd7cd9bb97ffa3cLL,
0xacf1faaebf7412LL, 0xf55d55d548bd86LL, 0x5b34112ed53d06LL, 0x1b58692e1e33b7LL, 0xcc7e3cb6d32fe2LL,
0x8f7b35c14a744fLL, 0x9a4ed599399554LL, 0x8eb369e71641afLL, 0xd4a6d1a5c74123LL, 0x8cc7ec376acf04LL,
0xec0a470647b248LL, 0x2fd9e8eea1f10eLL, 0x94439285677960LL, 0x4d11f6e6a426e0LL, 0x06378817230b68LL,
0xec14f2df152cb7LL, 0x199a8c0bd5f05dLL, 0xecad5aab44ac2bLL, 0xca87ab2ba6e905LL, 0x69c0bf2acdb36cLL,
0xd66279737bc807LL, 0x4dd946eb19d81bLL, 0x4e9c473b5e9846LL, 0x5a016f7ca86f9dLL, 0xd02c2b7dca744aLL}};

#define SOURCE 1
#define SINK 0

void compute_keys( unsigned long long Ksv, unsigned int source, unsigned long long *key ) {
  int i, j;
  //  unsigned long long key[40]; 
  int num;

  for( i = 0; i < 40; i++ ) {
    key[i] = 0LL;
  }
  if( source == SOURCE ) {
    printf( "source " );
  } else {
    printf( "sink " );
  }
    
  num = 0;
  //  printf( "key array for 0x%010llx, least bit first\n", Ksv );
  for( i = 0; i < 40; i++ ) { // i is a row counter
    if( Ksv & 1LL ) {
      num++;
      for( j = 0; j < 40; j++ ) { // j is a column counter
	if( source == SOURCE ) 
	  key[j] += hdcp_master[i][j];
	else
	  key[j] += hdcp_master[j][i];

	key[j] %= 72057594037927936LL;
      }
    }
    Ksv >>= 1LL;
  }
  printf ("KSV had %d ones\n", num);
  //  for( i = 0; i < 40; i++ ) {
  //      printf("0x%014llxLL, ", key[i]);
  //  }


}

#ifdef TESTING
#define KSV_PS3 0x7612e6f168LL
#define KSV_SAM 0xfad7307409LL

int main() {

  unsigned long long key[40]; 
/*
  printf( "hdcp master row 1\n" );
  for( i = 0; i < 40; i++ ) {
    printf( "%014llx, ", hdcp_master[1][i] );
  }

  printf( "hdcp master col 0\n" );
  for( i = 0; i < 40; i++ ) {
    printf( "%014llx, ", hdcp_master[i][0] );
  }
*/
  printf( "PS3: \n" );
  compute_keys( KSV_PS3, SOURCE, key );

  printf( "Samsung: \n" );
  compute_keys( KSV_SAM, SINK, key );

  return 0;
}
#endif
