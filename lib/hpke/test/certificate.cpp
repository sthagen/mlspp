#include <doctest/doctest.h>
#include <hpke/certificate.h>

#include "common.h"

#include <fstream>
#include <iostream>
#include <vector>

#include <tls/compat.h>
namespace opt = tls::opt;

TEST_CASE("Certificate Known-Answer depth 2")
{
  // TODO(suhas) Do this for each supported signature algorithm
  //      ... maybe including a case where parent and child have different
  //      algorithms
  // TODO(suhas) create different cert chains based on depth and algo

  // Chain is of depth 2
  const auto root_der = from_hex(
    "308201083081bba003020102021066144f6b1f7f06eaa3c5c4a24cdfb86f300506032b65"
    "7030143112301006035504031309746573742e636d6f6d301e170d323031303036303231"
    "3234395a170d3230313030373032313234395a3014311230100603550403130974657374"
    "2e636d6f6d302a300506032b65700321001afc1fc100f32f8abb6e7e1635eb873aba8583"
    "b8af948fb07e4b20376a8a89bba3233021300e0603551d0f0101ff0404030202a4300f06"
    "03551d130101ff040530030101ff300506032b6570034100a45de2d187cb28b4a74a4e82"
    "e4a000d68176ae68250803666d3a92b6595b0b0fbdcf231f83542fe29b74a95912a6b71b"
    "8e967f07df14b01b2b4779b233669e02");
  const auto issuing_der = from_hex(
    "3082010d3081c0a00302010202100cf69c4bee7caddc4bdcbd83d7f4e3e5300506032b65"
    "7030143112301006035504031309746573742e636d6f6d301e170d323031303036303231"
    "3234395a170d3230313030373032313234395a30193117301506035504030c0e022e696e"
    "742e746573742e636f6d302a300506032b6570032100ae385adcf6abd78d72bbcc1fdc94"
    "7903875e106c321a63d1367218d654356fd3a3233021300e0603551d0f0101ff04040302"
    "02a4300f0603551d130101ff040530030101ff300506032b65700341007520273c7a32fd"
    "b790c75e7925e53fb32c07add36f1d5c947209865d9ba88a7b2639466d3cf37e69df5667"
    "9d74700077906570c3690f74d944eaa2779ae0cb09");
  const auto leaf_der = from_hex(
    "3081f73081aaa003020102021100fad304f1a5a78be09d01347ed82a04e2300506032b65"
    "7030193117301506035504030c0e022e696e742e746573742e636f6d301e170d32303130"
    "30363032313234395a170d3230313030373032313234395a3000302a300506032b657003"
    "2100f8659f1bbfd057370f86c13c4dbe6850d2184a1b1a899d2d277a54d3666d7625a320"
    "301e300e0603551d0f0101ff0404030202a4300c0603551d130101ff0402300030050603"
    "2b6570034100ec538e976a425f1606e0e3d1f92599ab4a37fdd4deb07d3cf61a1f0f1867"
    "a0518253806c85a793ef5619b5803d4bc72a253a46f770acd65ae6907627e6852002");

  auto root = Certificate{ root_der };
  auto issuing = Certificate{ issuing_der };
  auto leaf = Certificate{ leaf_der };

  REQUIRE(!root.hash().empty());
  REQUIRE(!leaf.hash().empty());
  REQUIRE(!issuing.hash().empty());

  CHECK(root.raw == root_der);
  CHECK(issuing.raw == issuing_der);
  CHECK(leaf.raw == leaf_der);

  CHECK(leaf.valid_from(issuing));
  CHECK(issuing.valid_from(root));
  CHECK(root.valid_from(root));

  CHECK(!leaf.is_ca());
  CHECK(issuing.is_ca());
  CHECK(root.is_ca());

  REQUIRE(root.subject_hash() == 1072860458);
  REQUIRE(issuing.issuer_hash() == root.subject_hash());
  REQUIRE(leaf.issuer_hash() == issuing.subject_hash());

  // negative tests
  CHECK_FALSE(issuing.valid_from(leaf));
  CHECK_FALSE(root.valid_from(issuing));
  CHECK_FALSE(root.valid_from(leaf));
}

TEST_CASE("Certificate Known-Answer depth 2 with SKID/ADID")
{
  const auto root_der = from_hex(
    "308201183081cba0030201020211009561abf361bd738664041a79d918f602300506032b"
    "657030143112301006035504031309746573742e636d6f6d301e170d3230313030363035"
    "303433365a170d3230313030373035303433365a30143112301006035504031309746573"
    "742e636d6f6d302a300506032b657003210047f0149110ed81e2beaabbc3699527bdb8b7"
    "45da010da7fb8301d06fff8239e4a3323030300e0603551d0f0101ff0404030202a4300f"
    "0603551d130101ff040530030101ff300d0603551d0e04060404b9e672b8300506032b65"
    "70034100e15b54d50d1354f44017c5f8a037228546256c5fa1d750758fdf76f7e1dc246e"
    "7c67c18226ffd6704327bbae9a0cf5bd209facdcb524dc7efa517d1155487a0e");
  const auto issuing_der = from_hex(
    "3082012e3081e1a003020102021100919fc6cb4ea1a73766c95ada9a476aee300506032b"
    "657030143112301006035504031309746573742e636d6f6d301e170d3230313030363035"
    "303433365a170d3230313030373035303433365a30193117301506035504030c0e022e69"
    "6e742e746573742e636f6d302a300506032b6570032100c0a9d461880d82db662e984a8c"
    "06f74479817952070ea8cd0010971bc793ab9ca3433041300e0603551d0f0101ff040403"
    "0202a4300f0603551d130101ff040530030101ff300d0603551d0e0406040475ecb84430"
    "0f0603551d23040830068004b9e672b8300506032b6570034100e72bf92a39bab96649ab"
    "bd619c47b054bf7b071ceb24710ad253682d1df7690b0a1ef28ef8a2f76c3b8f2fed9d11"
    "3a61c98768db30d16fe6d9a36595775d110e");
  const auto leaf_der = from_hex(
    "308201163081c9a0030201020210291fb8fb96d2c215cb2d532f252d80e4300506032b65"
    "7030193117301506035504030c0e022e696e742e746573742e636f6d301e170d32303130"
    "30363035303433365a170d3230313030373035303433365a3000302a300506032b657003"
    "210032e4be5553d2141ace4da105fdf632da3467f013581f57dbd4f09706fa99949da340"
    "303e300e0603551d0f0101ff0404030202a4300c0603551d130101ff04023000300d0603"
    "551d0e04060404dd3b0790300f0603551d2304083006800475ecb844300506032b657003"
    "4100314a485d01df4c7852ec5720b2af34f5620b2a32a50c4ee0481d013ebbfd8e243784"
    "123a0cfe4d59b1fb09a1738ee9bc2aab59a2b4af2c3ee60ce19afbe1eb03");

  const auto root_skid = std::string("b9e672b8");
  const auto issuing_skid = std::string("75ecb844");
  const auto leaf_skid = std::string("dd3b0790");

  auto root = Certificate{ root_der };
  auto issuing = Certificate{ issuing_der };
  auto leaf = Certificate{ leaf_der };

  CHECK(root.raw == root_der);
  CHECK(issuing.raw == issuing_der);
  CHECK(leaf.raw == leaf_der);

  CHECK(leaf.valid_from(issuing));
  CHECK(issuing.valid_from(root));
  CHECK(root.valid_from(root));

  REQUIRE(leaf.subject_key_id().has_value());
  REQUIRE(leaf.authority_key_id().has_value());

  REQUIRE(issuing.subject_key_id().has_value());
  REQUIRE(issuing.authority_key_id().has_value());

  REQUIRE(root.subject_key_id().has_value());

  CHECK_EQ(to_hex(opt::get(leaf.subject_key_id())), leaf_skid);
  CHECK_EQ(to_hex(opt::get(leaf.authority_key_id())),
           to_hex(opt::get(issuing.subject_key_id())));
  CHECK_EQ(to_hex(opt::get(issuing.subject_key_id())), issuing_skid);
  CHECK_EQ(to_hex(opt::get(issuing.authority_key_id())),
           to_hex(opt::get(root.subject_key_id())));
  CHECK_EQ(to_hex(opt::get(root.subject_key_id())), root_skid);
}

TEST_CASE("Certificate Known-Answer depth 2 with SAN RFC822Name")
{
  const auto root_der = from_hex(
    "3081ff3081b2a00302010202101025963d9aefe2cdaf9c8017b9836b9b300506032b657030"
    "00301e170d3230313132353232333135365a170d3230313132363232333135365a3000302a"
    "300506032b65700321006fd52c993c4554c550c6f57a8c9b44834a99889c882e597d78e952"
    "afdbde748ea3423040300e0603551d0f0101ff0404030202a4300f0603551d130101ff0405"
    "30030101ff301d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d30"
    "0506032b6570034100accb5e7e05e607ca0c5a9103e962e360ea0b95ab8c876993af2660ef"
    "7e22ae6714f3d7b6b9594ac3eaaeeef263f764bc4939c84005db311ac4740b665694b004");
  const auto issuing_der = from_hex(
    "3081ff3081b2a0030201020210277bfa0157eaa84f1dc14c07ade455dd300506032b657030"
    "00301e170d3230313132353232333135365a170d3230313132363232333135365a3000302a"
    "300506032b65700321005ddafa25a2313f8dd19be29736825207a67282c2c6e327b8ac5127"
    "102e0d4eeda3423040300e0603551d0f0101ff0404030202a4300f0603551d130101ff0405"
    "30030101ff301d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d30"
    "0506032b6570034100eea828a18197fd4bd5751959318a7def21ce0c588b4107dc51ab6eb3"
    "e1a0a7c440cc019c186fbdbe227c0f368ab993c8a5af5c9681e11583d0442cafcaf01300");
  const auto leaf_der = from_hex(
    "3081fd3081b0a003020102021100af5442db77d60c749fffe8eebf193afa300506032b6570"
    "3000301e170d3230313132353232333135365a170d3230313132363232333135365a300030"
    "2a300506032b6570032100885cc6836723e204b54275c97928481c55b149e1ed0e22b30d2f"
    "1a89aa24e2d1a33f303d300e0603551d0f0101ff0404030202a4300c0603551d130101ff04"
    "023000301d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d300506"
    "032b65700341002cc5b3f1a8954ccc872ecddf5779fb007c08ebc869227dec09cfba8fd977"
    "ea49a182a2e51b67d4440d42248f6951f4c765e9e72e301225c953e89b2747129a0c");

  auto root = Certificate{ root_der };
  auto issuing = Certificate{ issuing_der };
  auto leaf = Certificate{ leaf_der };

  CHECK(leaf.valid_from(issuing));
  CHECK(issuing.valid_from(root));
  CHECK(root.valid_from(root));

  CHECK_EQ(leaf.email_addresses().at(0), "user@domain.com");
}

TEST_CASE("RSA Certificate Known-Answer depth 2")
{
  const auto root_der = from_hex(
    "308202dc308201c4a0030201020211009f3b55576e66866d81ac2aff814dd078300d06092a"
    "864886f70d01010b05003000301e170d3231303130383033333533385a170d323130313039"
    "3033333533385a300030820122300d06092a864886f70d01010105000382010f003082010a"
    "0282010100e406fa017bbc10efda6b2c04b1b6c02a2b7023afab14f39c0e191601aff0b180"
    "08b4c8baf623d94f3ef535398a6507ac0d6af8220a57ce01aedeca1685c817abc4210a779b"
    "951079b02499a9c01247a7a778c8cdffff372910dbf09a416627cf195a3de975d7d424372f"
    "c8f7977aa21c16549373b13d52356e2ade101632edc5dbfc405399f4bbfbd7d2e0128c6b33"
    "bebc9b7b471d1e39c4d94930c941a205c30f1529fe63adbd71a4354cd0e6c3c29f42c10ed3"
    "5fbba07319e8d6d251ee89fcfb1ed280d5614b50f703d13adc04743c182fd8240d318b3008"
    "cae0643d0253aea819ded8281d5d68cbf241a16b9208e2de649bbb2f2a9a0be09b48ff2cc7"
    "e39b0203010001a351304f300e0603551d0f0101ff0404030202a4300f0603551d130101ff"
    "040530030101ff300d0603551d0e0406040496014f01301d0603551d110101ff0413301181"
    "0f7573657240646f6d61696e2e636f6d300d06092a864886f70d01010b05000382010100bf"
    "36d99f9e33360cbc1a6ac5cc3a7fde03ec6f6c5ca397bcd1f26b359c7e6b1f18ceaad7a901"
    "9f517bac8c1f391735a290c0fc7a8c82d19ed39da400f4fd3799cb9b0fb915bb47b10bf70d"
    "3bb1b94b534c2cc17c033d78cbef82c62c634f72ea939bee9127feb0b510cb616af2aaeb68"
    "7e984a414c1459ea2053836c28ad0bdb1bea034ea53bc45aa242e706f5bd60c20bfbd3b438"
    "3aefa38faf51aca860f217a580cecca5a9a17c1a469e635b3e5d35f44b4480d5b744abf149"
    "809886f71df9b38c3d464cc28dc9f7b5db0883b8acdd5a620b75abfbeb50b8e07258edd21b"
    "4390e220d2826074409468d80332d5ce593321dabd68c24ca2b26f09881632e81c");
  const auto issuing_der = from_hex(
    "308202db308201c3a003020102021055fe8d63eed32050a48a7759f502ea23300d06092a86"
    "4886f70d01010b05003000301e170d3231303130383033333533385a170d32313031303930"
    "33333533385a300030820122300d06092a864886f70d01010105000382010f003082010a02"
    "82010100c192285946858f865ec8d72fc14a4bd8d90e3ae4ff796f8d5c07d50613d7435251"
    "a9d29866d00ea26a6b3475201c3774e720d55d9043a21c2c3d7a2e7ae385202448895fb287"
    "19db9a21e27de8d8a13e45b10a9ccdc64a602e7fbe2247eb67c092e6210ccc860d7185e117"
    "e65ab497b03cb82aa1d15116b5fbd5883c0c59eca5ca0bc2a339af41f5c2040fd3f15a6616"
    "46b25bfaf7cac060b0cfcd32e641c80e1eeb200ef748b4bbf71626e5c5cfd38e36a6ac2468"
    "9c822c7bd9bd5ce81c8267ebc8aa1da34cc80901b905b9fd5905f8d38e350a190d82f8a259"
    "0f50a64bf514f1f1e3eb517d7b67f58bbc3f924705037d78c24b5b35cc457a651e73c47b6b"
    "970203010001a351304f300e0603551d0f0101ff0404030202a4300f0603551d130101ff04"
    "0530030101ff300d0603551d0e04060404f375df9a301d0603551d110101ff04133011810f"
    "7573657240646f6d61696e2e636f6d300d06092a864886f70d01010b05000382010100485b"
    "3866b842590ceeeb36b861a1fd6aa980f7f01982f622c08bf61f10c7bba098bb63d395338a"
    "b6dbd80d93dca2bd4d221342cc0d0b370b4fa4663d1a7189d7d0e048fe0a2ff642e16e5050"
    "e2b02541457573de067f6ff6da9d686f4cf29b29fd06739e166e74e20e6471820515ca51e5"
    "7f0bf749036a48d69d8bf17aa48dadc59f3a5042ee5ba03915bdb25aaf1bc045574fec8ded"
    "e70f202a84004321bb4eca4cc6f41bf9152374abb1964f384af33ffccd052d2349f4968945"
    "4f3e486aa8af9a585b37339472ffab13c8eec91cfef52fecc474e76ebae097f35982646a48"
    "a9dd59afcb5109d9aec181f41db10993cfa1260aee0225238430beecd2fbc1af");
  const auto leaf_der = from_hex(
    "308202d8308201c0a00302010202106e29bc66c0bd4beecdcfca0206dc0fd7300d06092a86"
    "4886f70d01010b05003000301e170d3231303130383033333533385a170d32313031303930"
    "33333533385a300030820122300d06092a864886f70d01010105000382010f003082010a02"
    "82010100bce6b1c1698ac7814bb2a90f2bf2f312fea68005de63f3c351a3c177666199b77a"
    "4b4c0862ec86694e3a5133d969b80583676a1b439f75546ab5b521e1254bccf8f2533f452d"
    "2cc94656c277f5e2de683c2b1824c3ffa2ead581c17cf8207b81f8803a7fa6e477de7e0bd4"
    "b02262ee3a8cbfee2b4b57bb3626ad13e12a6ac4e781c45371deb2bda233f054aefd6c3618"
    "47d7e1a5757fa4bbeed8712871f22bc46c383491ed3c3718896298e6bcd498f7fbcc6af292"
    "78a5ed5a95a65ee03892a166559332621de95dbfd45979099e788d61c98683ec39b7c4fbb7"
    "59bf08b9f74baaf157a82f4e6f26eb16f684be49af9d58bfb4f0ff65f924d0b3994b4ea761"
    "8f0203010001a34e304c300e0603551d0f0101ff0404030202a4300c0603551d130101ff04"
    "023000300d0603551d0e0406040409682fe0301d0603551d110101ff04133011810f757365"
    "7240646f6d61696e2e636f6d300d06092a864886f70d01010b05000382010100949426508f"
    "08f3c05219d736234cddda0dfc6da3973ded235755bf5ed427cd61c041dcc3671c0acd82c1"
    "5cc297debb60d9bdd3e4350e2c62c8adb241281e9907b1f6d6171ab3e48ca49dacb43d9828"
    "844249d28973edc5d99e7d1c94809745ec6d6a0360a5bad6c0f51fcf44c50edc00d12273d2"
    "ba580cbabd112d2338e83c1cf8c3ddfee8ec0aac6c6251c224b5ae92153afa8cd33d2dab05"
    "46213b63501aba095ddb5ab43699ef2aa201b4552b2aa97f15ca7d8708f630370a9cdaa182"
    "59ccf1736a59d8d4da9969d7ae06be41cce68fdd263a5c9a9658e80d9a4ad6bf51f1a39082"
    "4059cf88a84fd70b72349602d3224b9878e72d0e7e7cffea35fcf36da5");

  const auto root_skid = std::string("b9e672b8");
  const auto issuing_skid = std::string("75ecb844");
  const auto leaf_skid = std::string("dd3b0790");

  auto root = Certificate{ root_der };
  auto issuing = Certificate{ issuing_der };
  auto leaf = Certificate{ leaf_der };

  CHECK(root.raw == root_der);
  CHECK(issuing.raw == issuing_der);
  CHECK(leaf.raw == leaf_der);

  CHECK(leaf.valid_from(issuing));
  CHECK(issuing.valid_from(root));
  CHECK(root.valid_from(root));

  CHECK(!leaf.is_ca());
  CHECK(issuing.is_ca());
  CHECK(root.is_ca());

  REQUIRE(issuing.issuer_hash() == root.subject_hash());
  REQUIRE(leaf.issuer_hash() == issuing.subject_hash());

  // negative tests
  CHECK_FALSE(issuing.valid_from(leaf));
  CHECK_FALSE(root.valid_from(issuing));
  CHECK_FALSE(root.valid_from(leaf));
}

TEST_CASE("IdenTrust Root CA with sha1")
{
  const auto root_der = from_hex(
    "3082034a30820232a003020102021044afb080d6a327ba893039862ef8406b300d06092a86"
    "4886f70d0101050500303f31243022060355040a131b4469676974616c205369676e617475"
    "726520547275737420436f2e311730150603550403130e44535420526f6f74204341205833"
    "301e170d3030303933303231313231395a170d3231303933303134303131355a303f312430"
    "22060355040a131b4469676974616c205369676e617475726520547275737420436f2e3117"
    "30150603550403130e44535420526f6f7420434120583330820122300d06092a864886f70d"
    "01010105000382010f003082010a0282010100dfafe99750088357b4cc6265f69082ecc7d3"
    "2c6b30ca5becd9c37dc740c118148be0e83376492ae33f214993ac4e0eaf3e48cb65eefcd3"
    "210f65d22ad9328f8ce5f777b0127bb595c089a3a9baed732e7a0c063283a27e8a1430cd11"
    "a0e12a38b9790a31fd50bd8065dfb7516383c8e28861ea4b6181ec526bb9a2e24b1a289f48"
    "a39e0cda098e3e172e1edd20df5bc62a8aab2ebd70adc50b1a25907472c57b6aab34d63089"
    "ffe568137b540bc8d6aeec5a9c921e3d64b38cc6dfbfc94170ec1672d526ec38553943d0fc"
    "fd185c40f197ebd59a9b8d1dbada25b9c6d8dfc115023aabda6ef13e2ef55c089c3cd68369"
    "e4109b192ab62957e3e53d9b9ff0025d0203010001a3423040300f0603551d130101ff0405"
    "30030101ff300e0603551d0f0101ff040403020106301d0603551d0e04160414c4a7b1a47b"
    "2c71fadbe14b9075ffc41560858910300d06092a864886f70d01010505000382010100a31a"
    "2c9b17005ca91eee2866373abf83c73f4bc309a095205de3d95944d23e0d3ebd8a4ba0741f"
    "ce10829c741a1d7e981addcb134bb32044e491e9ccfc7da5db6ae5fee6fde04eddb7003ab5"
    "7049aff2e5eb02f1d1028b19cb943a5e48c4181e58195f1e025af00cf1b1ada9dc59868b6e"
    "e991f586cafab96633aa595bcee2a7167347cb2bcc99b03748cfe3564bf5cf0f0c723287c6"
    "f044bb53726d43f526489a5267b758abfe67767178db0da256141339243185a2a8025a3047"
    "e1dd5007bc02099000eb6463609b16bc88c912e6d27d918bf93d328d65b4e97cb15776eac5"
    "b62839bf15651cc8f677966a0a8d770bd8910b048e07db29b60aee9d82353510");

  auto root = Certificate{ root_der };

  CHECK(root.raw == root_der);
  CHECK(root.valid_from(root));
  CHECK(root.is_ca());
}

TEST_CASE("Ecdsa p256 Certificate")
{
  // ecdsa-p256 cert chain
  const auto root_der = from_hex(
    "3082014d3081f5a003020102021038247e1ba3468e9913ab357d9e106abe300a06082a8648"
    "ce3d0403023000301e170d3231303132393035343232305a170d3231303133303035343232"
    "305a30003059301306072a8648ce3d020106082a8648ce3d03010703420004dea4dc4bb1c9"
    "824c471efeaab03f74eb243e15388cd5df912ad3ab46c97351a41323975a9c985fbd95d78a"
    "40b4411368ce9a4cd4db7eb3c3962d59870cb783bfa351304f300e0603551d0f0101ff0404"
    "030202a4300f0603551d130101ff040530030101ff300d0603551d0e04060404927b64c630"
    "1d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d300a06082a8648"
    "ce3d0403020347003044022078e8cab207d4cf86010a5e2193d47c150c3faebe7f45fcadd7"
    "225c4c80d7ee2302203373b4d283d0417de2a3f27b3470b0137b80001715f99e05284e8b16"
    "ed288540");
  const auto issuing_der = from_hex(
    "3082014f3081f5a00302010202101397ccc4e13819f59adffea8f5aa55ba300a06082a8648"
    "ce3d0403023000301e170d3231303132393035343232305a170d3231303133303035343232"
    "305a30003059301306072a8648ce3d020106082a8648ce3d030107034200045e210200da76"
    "c73e9da9c9c3ff68fb680d12b60633c29cc4ec8aa66e4be2cde4f21e706dd64aaa2adda116"
    "f4d5474d6689a7205835fd5a13ecd46b64a71c807ca351304f300e0603551d0f0101ff0404"
    "030202a4300f0603551d130101ff040530030101ff300d0603551d0e04060404f312fa8f30"
    "1d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d300a06082a8648"
    "ce3d0403020349003046022100a4f4defca4ed13b781b797707670bdf885d1aa6cf8a5d9ba"
    "586066ada8cca961022100e26d5a5c7cf169e820bb3c5fb1a8e8a9f48678d46cf7d489270c"
    "98546efa46fb");
  const auto leaf_der = from_hex(
    "3082014a3081f2a003020102021003d61b85fa16a9439b2a9c9dcd9ee575300a06082a8648"
    "ce3d0403023000301e170d3231303132393035343232305a170d3231303133303035343232"
    "305a30003059301306072a8648ce3d020106082a8648ce3d03010703420004177f3e44f97d"
    "36df7e7216857e4ce511a2e402376a59d9887b90c59351bd7cca387c0cc22a25038a1532df"
    "ec82b49661ad38ad21acfaca9adb524e3761b1943fa34e304c300e0603551d0f0101ff0404"
    "030202a4300c0603551d130101ff04023000300d0603551d0e04060404db6458df301d0603"
    "551d110101ff04133011810f7573657240646f6d61696e2e636f6d300a06082a8648ce3d04"
    "0302034700304402201dc0f17ab8f980bb8a13ad1003325fe237af78f7338ef2dbf383a493"
    "191217d402203505672d488ea336a70be868ceb69dbd163f437942fb4ea3ef1a844cb4927d"
    "0e");

  auto root = Certificate{ root_der };
  auto issuing = Certificate{ issuing_der };
  auto leaf = Certificate{ leaf_der };

  REQUIRE(!root.hash().empty());
  REQUIRE(!leaf.hash().empty());
  REQUIRE(!issuing.hash().empty());

  CHECK(root.raw == root_der);
  CHECK(issuing.raw == issuing_der);
  CHECK(leaf.raw == leaf_der);

  CHECK(leaf.valid_from(issuing));
  CHECK(issuing.valid_from(root));
  CHECK(root.valid_from(root));

  CHECK(!leaf.is_ca());
  CHECK(issuing.is_ca());
  CHECK(root.is_ca());

  // negative tests
  CHECK_FALSE(issuing.valid_from(leaf));
  CHECK_FALSE(root.valid_from(issuing));
  CHECK_FALSE(root.valid_from(leaf));
}

TEST_CASE("PEM parsing")
{
  // ecdsa-p256 cert chain
  const auto root_der = from_hex(
    "3082014d3081f5a003020102021038247e1ba3468e9913ab357d9e106abe300a06082a8648"
    "ce3d0403023000301e170d3231303132393035343232305a170d3231303133303035343232"
    "305a30003059301306072a8648ce3d020106082a8648ce3d03010703420004dea4dc4bb1c9"
    "824c471efeaab03f74eb243e15388cd5df912ad3ab46c97351a41323975a9c985fbd95d78a"
    "40b4411368ce9a4cd4db7eb3c3962d59870cb783bfa351304f300e0603551d0f0101ff0404"
    "030202a4300f0603551d130101ff040530030101ff300d0603551d0e04060404927b64c630"
    "1d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d300a06082a8648"
    "ce3d0403020347003044022078e8cab207d4cf86010a5e2193d47c150c3faebe7f45fcadd7"
    "225c4c80d7ee2302203373b4d283d0417de2a3f27b3470b0137b80001715f99e05284e8b16"
    "ed288540");
  const auto issuing_der = from_hex(
    "3082014f3081f5a00302010202101397ccc4e13819f59adffea8f5aa55ba300a06082a8648"
    "ce3d0403023000301e170d3231303132393035343232305a170d3231303133303035343232"
    "305a30003059301306072a8648ce3d020106082a8648ce3d030107034200045e210200da76"
    "c73e9da9c9c3ff68fb680d12b60633c29cc4ec8aa66e4be2cde4f21e706dd64aaa2adda116"
    "f4d5474d6689a7205835fd5a13ecd46b64a71c807ca351304f300e0603551d0f0101ff0404"
    "030202a4300f0603551d130101ff040530030101ff300d0603551d0e04060404f312fa8f30"
    "1d0603551d110101ff04133011810f7573657240646f6d61696e2e636f6d300a06082a8648"
    "ce3d0403020349003046022100a4f4defca4ed13b781b797707670bdf885d1aa6cf8a5d9ba"
    "586066ada8cca961022100e26d5a5c7cf169e820bb3c5fb1a8e8a9f48678d46cf7d489270c"
    "98546efa46fb");
  const auto leaf_der = from_hex(
    "3082014a3081f2a003020102021003d61b85fa16a9439b2a9c9dcd9ee575300a06082a8648"
    "ce3d0403023000301e170d3231303132393035343232305a170d3231303133303035343232"
    "305a30003059301306072a8648ce3d020106082a8648ce3d03010703420004177f3e44f97d"
    "36df7e7216857e4ce511a2e402376a59d9887b90c59351bd7cca387c0cc22a25038a1532df"
    "ec82b49661ad38ad21acfaca9adb524e3761b1943fa34e304c300e0603551d0f0101ff0404"
    "030202a4300c0603551d130101ff04023000300d0603551d0e04060404db6458df301d0603"
    "551d110101ff04133011810f7573657240646f6d61696e2e636f6d300a06082a8648ce3d04"
    "0302034700304402201dc0f17ab8f980bb8a13ad1003325fe237af78f7338ef2dbf383a493"
    "191217d402203505672d488ea336a70be868ceb69dbd163f437942fb4ea3ef1a844cb4927d"
    "0e");

  const auto pem_string = std::string(R"pem(-----BEGIN CERTIFICATE-----
MIIBTTCB9aADAgECAhA4JH4bo0aOmROrNX2eEGq+MAoGCCqGSM49BAMCMAAwHhcN
MjEwMTI5MDU0MjIwWhcNMjEwMTMwMDU0MjIwWjAAMFkwEwYHKoZIzj0CAQYIKoZI
zj0DAQcDQgAE3qTcS7HJgkxHHv6qsD906yQ+FTiM1d+RKtOrRslzUaQTI5danJhf
vZXXikC0QRNozppM1Nt+s8OWLVmHDLeDv6NRME8wDgYDVR0PAQH/BAQDAgKkMA8G
A1UdEwEB/wQFMAMBAf8wDQYDVR0OBAYEBJJ7ZMYwHQYDVR0RAQH/BBMwEYEPdXNl
ckBkb21haW4uY29tMAoGCCqGSM49BAMCA0cAMEQCIHjoyrIH1M+GAQpeIZPUfBUM
P66+f0X8rdciXEyA1+4jAiAzc7TSg9BBfeKj8ns0cLATe4AAFxX5ngUoTosW7SiF
QA==
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIBTzCB9aADAgECAhATl8zE4TgZ9Zrf/qj1qlW6MAoGCCqGSM49BAMCMAAwHhcN
MjEwMTI5MDU0MjIwWhcNMjEwMTMwMDU0MjIwWjAAMFkwEwYHKoZIzj0CAQYIKoZI
zj0DAQcDQgAEXiECANp2xz6dqcnD/2j7aA0StgYzwpzE7IqmbkvizeTyHnBt1kqq
Kt2hFvTVR01miacgWDX9WhPs1GtkpxyAfKNRME8wDgYDVR0PAQH/BAQDAgKkMA8G
A1UdEwEB/wQFMAMBAf8wDQYDVR0OBAYEBPMS+o8wHQYDVR0RAQH/BBMwEYEPdXNl
ckBkb21haW4uY29tMAoGCCqGSM49BAMCA0kAMEYCIQCk9N78pO0Tt4G3l3B2cL34
hdGqbPil2bpYYGatqMypYQIhAOJtWlx88WnoILs8X7Go6Kn0hnjUbPfUiScMmFRu
+kb7
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIBSjCB8qADAgECAhAD1huF+hapQ5sqnJ3NnuV1MAoGCCqGSM49BAMCMAAwHhcN
MjEwMTI5MDU0MjIwWhcNMjEwMTMwMDU0MjIwWjAAMFkwEwYHKoZIzj0CAQYIKoZI
zj0DAQcDQgAEF38+RPl9Nt9+chaFfkzlEaLkAjdqWdmIe5DFk1G9fMo4fAzCKiUD
ihUy3+yCtJZhrTitIaz6yprbUk43YbGUP6NOMEwwDgYDVR0PAQH/BAQDAgKkMAwG
A1UdEwEB/wQCMAAwDQYDVR0OBAYEBNtkWN8wHQYDVR0RAQH/BBMwEYEPdXNlckBk
b21haW4uY29tMAoGCCqGSM49BAMCA0cAMEQCIB3A8Xq4+YC7ihOtEAMyX+I3r3j3
M47y2/ODpJMZEhfUAiA1BWctSI6jNqcL6GjOtp29Fj9DeUL7TqPvGoRMtJJ9Dg==
-----END CERTIFICATE-----
)pem");

  const auto from_der = std::vector<Certificate>{ Certificate{ root_der },
                                                  Certificate{ issuing_der },
                                                  Certificate{ leaf_der } };

  const auto pem_bytes = bytes(pem_string.begin(), pem_string.end());
  const auto from_pem = Certificate::parse_pem(pem_bytes);

  REQUIRE(from_der == from_pem);
}

TEST_CASE("Test Subject Parsing")
{
  const auto leaf_der = from_hex(
    "3082015e30820110a003020102021100d4a0be6c42e855fa6df8269a5521747f300506032b"
    "6570302a311530130603550403130c637573746f6d3a31323334353111300f060355040513"
    "0831312d32322d3333301e170d3231303231313233353033325a170d323130323132323335"
    "3033325a302a311530130603550403130c637573746f6d3a31323334353111300f06035504"
    "05130831312d32322d3333302a300506032b6570032100b6f359d48609b81ff3eee3546c23"
    "5a5a31c8dd1b84c29bf0d9ea32fad3945299a34b3049300e0603551d0f0101ff0404030202"
    "a4300c0603551d130101ff04023000300d0603551d0e040604048dc683c3301a0603551d11"
    "04133011810f7573657240646f6d61696e2e636f6d300506032b6570034100dcbc8d3431f8"
    "2f7cc7c97672bad001119b6b4bfbd9ee96a1096238d59ff3211529a90a6b148ed874ca1349"
    "5e636388ef623f486c85dc53c3e2377809d7fda004");

  auto leaf = Certificate{ leaf_der };
  CHECK(leaf.raw == leaf_der);
  auto parsed_subject = leaf.subject();
  CHECK_EQ(parsed_subject.size(), 2);
  auto it = parsed_subject.find(Certificate::NameType::common_name);
  CHECK_EQ(it->second, "custom:12345");
  it = parsed_subject.find(Certificate::NameType::serial_number);
  CHECK_EQ(it->second, "11-22-33");
}

TEST_CASE("Test Certificate notBefore status")
{
  // notBefore - 99 years from 04/22/2021
  const auto root_der = from_hex(
    "3082016230820114a00302010202101dcfbd024e5f62ccfb04a1f32e7ce755300506032b65"
    "70302a311530130603550403130c637573746f6d3a31323334353111300f06035504051308"
    "31312d32322d33333020170d3236303330383036333631345a180f32313235303231323036"
    "333631345a302a311530130603550403130c637573746f6d3a31323334353111300f060355"
    "0405130831312d32322d3333302a300506032b6570032100cb6d233470f884eaa6e5a3b958"
    "e7e68eff3fee146432ea526128171a33f8a403a34e304c300e0603551d0f0101ff04040302"
    "02a4300f0603551d130101ff040530030101ff300d0603551d0e0406040459e6b3dd301a06"
    "03551d1104133011810f7573657240646f6d61696e2e636f6d300506032b6570034100308a"
    "a37cfbcd06e19b7a0728c5c970b38df5eb93d478970868ce6398a6a963b2c570edfd9dc62f"
    "4d134de11eca367f9d967d6eae14192454770a2fc278963602");

  auto root = Certificate{ root_der };
  REQUIRE(root.expiration_status() == Certificate::ExpirationStatus::inactive);
}

TEST_CASE("Test Certificate notAfter status")
{
  // notAfter - 2 days older than 04/22/2021
  const auto root_der = from_hex(
    "3082016030820112a00302010202104746a8dc01b0c2a729cb307bd52cd94f300506032b65"
    "70302a311530130603550403130c637573746f6d3a31323334353111300f06035504051308"
    "31312d32322d3333301e170d3231303431383036333831385a170d32313034323130363338"
    "31385a302a311530130603550403130c637573746f6d3a31323334353111300f0603550405"
    "130831312d32322d3333302a300506032b65700321003beb77d095e9bb842d52f3642e4955"
    "b907f1e42129be39db81059860e6ea5c27a34e304c300e0603551d0f0101ff0404030202a4"
    "300f0603551d130101ff040530030101ff300d0603551d0e04060404b2e294f2301a060355"
    "1d1104133011810f7573657240646f6d61696e2e636f6d300506032b6570034100ba82b764"
    "43b126cab7db4a8a054e53ee25ef39d1ef2642931c7961c5591643edf73b25fc2bd7c3527b"
    "e4e7d2b0606050b2e0edcfc8d6390b373e21f08116910b");

  auto root = Certificate{ root_der };
  REQUIRE(root.expiration_status() == Certificate::ExpirationStatus::expired);
}

TEST_CASE("Test Certificate active status")
{
  // notBefore - 2 days older than 04/22/2021
  // notAfter - 99 years from 04/22/2021
  const auto root_der = from_hex(
    "3082016230820114a0030201020210021dca182b9f52f6081841be66a84938300506032b65"
    "70302a311530130603550403130c637573746f6d3a31323334353111300f06035504051308"
    "31312d32322d33333020170d3231303432313036343132385a180f32313230303333303036"
    "343132385a302a311530130603550403130c637573746f6d3a31323334353111300f060355"
    "0405130831312d32322d3333302a300506032b65700321004b16ffec9d6db170f06ca73ec7"
    "64e206c71c57c3f7454b4bd17bcef26c22b7f7a34e304c300e0603551d0f0101ff04040302"
    "02a4300f0603551d130101ff040530030101ff300d0603551d0e0406040488fbad09301a06"
    "03551d1104133011810f7573657240646f6d61696e2e636f6d300506032b6570034100c644"
    "939be90b8121a7e5b371396f89b01235dd8cde01e88d6d7e09fb02ae6c1bb5e5abfe21de96"
    "16a821444907e84cd4fb88167f1c3a4d4911f8260dafb21b05");

  auto root = Certificate{ root_der };
  REQUIRE(root.expiration_status() == Certificate::ExpirationStatus::active);
  CHECK_NOTHROW(root.not_before());
  CHECK_NOTHROW(root.not_after());
  REQUIRE(root.signature_algo() == "ed25519");
  REQUIRE(root.public_key_algo() == "ed25519");
}