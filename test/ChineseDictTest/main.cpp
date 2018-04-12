#include "tst_pinyinutils.h"

#include <gtest/gtest.h>
#include "textutils.h"
#include <QDebug>


int main(int argc, char *argv[])
{

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(PinyinUtils, determineTextFormat) {
    ASSERT_EQ(tfHanzi, determineTextFormat(u8"价廉物美"));
    ASSERT_EQ(tfHanzi, determineTextFormat(u8"ＵＳＢ手指"));
    ASSERT_EQ(tfPinyinNumbers, determineTextFormat("jia4 lian2 wu4 mei3"));
    ASSERT_EQ(tfPinyinNoTones, determineTextFormat("jia lian wu mei"));
    ASSERT_EQ(tfPinyinNoTones, determineTextFormat("jialianwumei"));
    ASSERT_EQ(tfPinyinTonemarks, determineTextFormat(u8"jià lián wù měi"));
    ASSERT_EQ(tfPinyinNoTones, determineTextFormat(u8"lüe"));
    ASSERT_EQ(tfPinyinNoTones, determineTextFormat(u8"LÜE"));
    ASSERT_EQ(tfPinyinTonemarks, determineTextFormat(u8"lüē"));
    ASSERT_EQ(tfPinyinTonemarks, determineTextFormat(u8"LÜĒ"));
    ASSERT_EQ(tfPinyinTonemarks, determineTextFormat(u8"shì"));
    ASSERT_EQ(tfPinyinTonemarks, determineTextFormat(u8"shì shi"));
}

QString extractToneNumbers(const QString& source)
{
    QString displayPinyin, tonemarkedSearchPinyin, tonelessSearchPinyin, toneNums, componentPinyin;
    parseCedictEntry(source, displayPinyin, tonemarkedSearchPinyin, tonelessSearchPinyin, toneNums, componentPinyin);
    assert(toneNums.split(',').length() == componentPinyin.split(',').length());
    return toneNums;
}


TEST(PinyinUtils, extractToneNumbers) {
    ASSERT_EQ("1,2,3,4,5,5", extractToneNumbers("bei1 bei2 bei3 bei4 bei5 bei"));
    ASSERT_EQ("1,2,3,4,5,5", extractToneNumbers("shui1 shui2 shui3 shui4 shui5 shui"));
    ASSERT_EQ("1,1,2,2,3,3,4,4,5,5,5,5", extractToneNumbers("lu:1 lv1 lu:2 lv2 lu:3 lv3 lu:4 lv4 lv5 lu:5 lv lu:"));
}

// I realise not all of these are valid chinese words

TEST(PinyinUtils, display_lowercase) {
    ASSERT_EQ("wen lü", makeDisplayPinyin("wen lu:"));
    ASSERT_EQ("wén lǚ", makeDisplayPinyin("wen2 lu:3"));
    ASSERT_EQ("qīn zǐ jiàn dìng", makeDisplayPinyin("qin1 zi3 jian4 ding4"));
    ASSERT_EQ("jià lián wù měi", makeDisplayPinyin("jia4 lian2 wu4 mei3"));
    ASSERT_EQ("wēi wàng", makeDisplayPinyin("wei1 wang4"));
    ASSERT_EQ("xiè mò shā lǘ", makeDisplayPinyin("xie4 mo4 sha1 lv2"));
    ASSERT_EQ("Lüè duó zhě", makeDisplayPinyin("Lve4 duo2 zhe3"));
    ASSERT_EQ("Lüè duó zhě", makeDisplayPinyin("Lu:e4 duo2 zhe3"));
    ASSERT_EQ("Shàng hǎi", makeDisplayPinyin("Shang4 hai3"));
    ASSERT_EQ("zī liào", makeDisplayPinyin("zi1 liao4"));
    ASSERT_EQ("jiè shào", makeDisplayPinyin("jie4 shao4"));


    ASSERT_EQ(u8"bēi béi běi bèi bei bei", makeDisplayPinyin("bei1 bei2 bei3 bei4 bei5 bei"));
    ASSERT_EQ(u8"bēibéiběibèibeibei", makeToneMarkedSearchPinyin("bei1bei2bei3bei4bei5bei"));
    ASSERT_EQ(u8"jiā jiá jiǎ jià jia jia", makeDisplayPinyin("jia1 jia2 jia3 jia4 jia5 jia"));
    ASSERT_EQ(u8"jiājiájiǎjiàjiajia", makeToneMarkedSearchPinyin("jia1jia2jia3jia4jia5jia"));
    ASSERT_EQ(u8"jiē jié jiě jiè jie jie", makeDisplayPinyin("jie1 jie2 jie3 jie4 jie5 jie"));
    ASSERT_EQ(u8"liū liú liǔ liù liu liu", makeDisplayPinyin("liu1 liu2 liu3 liu4 liu5 liu"));
    ASSERT_EQ(u8"xiōng xióng xiǒng xiòng xiong xiong", makeDisplayPinyin("xiong1 xiong2 xiong3 xiong4 xiong5 xiong"));
    ASSERT_EQ(u8"xiōngxióngxiǒngxiòngxiongxiong", makeToneMarkedSearchPinyin("xiong1xiong2xiong3xiong4xiong5xiong"));
    ASSERT_EQ(u8"huā huá huǎ huà hua hua", makeDisplayPinyin("hua1 hua2 hua3 hua4 hua5 hua"));
    ASSERT_EQ(u8"yuē yué yuě yuè yue yue", makeDisplayPinyin("yue1 yue2 yue3 yue4 yue5 yue"));
    ASSERT_EQ(u8"shuī shuí shuǐ shuì shui shui", makeDisplayPinyin("shui1 shui2 shui3 shui4 shui5 shui"));
    ASSERT_EQ(u8"duī duí duǐ duì dui dui", makeDisplayPinyin("dui1 dui2 dui3 dui4 dui5 dui"));
    ASSERT_EQ(u8"duō duó duǒ duò duo duo", makeDisplayPinyin("duo1 duo2 duo3 duo4 duo5 duo"));
    //ASSERT_EQ(u8"lüē lüē lüé lüé lüě lüě lüè lüè lüe lüe lüe lüe", makeDisplayPinyin("lve1 lu:e1 lve2 lu:e2 lve3 lu:e3 lve4 lu:e4 lve5 lu:e5 lve lu:e"));
    ASSERT_EQ(u8"lüēlüēlüélüélüělüělüèlüèlüelüelüelüe", makeToneMarkedSearchPinyin("lve1lu:e1lve2lu:e2lve3lu:e3lve4lu:e4lve5lu:e5lvelu:e"));
    ASSERT_EQ(u8"mā má mǎ mà ma ma", makeDisplayPinyin("ma1 ma2 ma3 ma4 ma5 ma"));
    ASSERT_EQ(u8"cē cé cě cè ce ce", makeDisplayPinyin("ce1 ce2 ce3 ce4 ce5 ce"));
    ASSERT_EQ(u8"cī cí cǐ cì ci ci", makeDisplayPinyin("ci1 ci2 ci3 ci4 ci5 ci"));
    ASSERT_EQ(u8"bō bó bǒ bò bo bo", makeDisplayPinyin("bo1 bo2 bo3 bo4 bo5 bo"));
    ASSERT_EQ(u8"bū bú bǔ bù bu bu", makeDisplayPinyin("bu1 bu2 bu3 bu4 bu5 bu"));
    ASSERT_EQ(u8"lǖ lǖ lǘ lǘ lǚ lǚ lǜ lǜ lü lü lü lü", makeDisplayPinyin("lu:1 lv1 lu:2 lv2 lu:3 lv3 lu:4 lv4 lv5 lu:5 lv lu:"));

}


TEST(PinyinUtils, display_uppercase) {
    ASSERT_EQ(u8"BĒI BÉI BĚI BÈI BEI BEI", makeDisplayPinyin("BEI1 BEI2 BEI3 BEI4 BEI5 BEI"));
    ASSERT_EQ(u8"JIĀ JIÁ JIǍ JIÀ JIA JIA", makeDisplayPinyin("JIA1 JIA2 JIA3 JIA4 JIA5 JIA"));
    ASSERT_EQ(u8"JIĒ JIÉ JIĚ JIÈ JIE JIE", makeDisplayPinyin("JIE1 JIE2 JIE3 JIE4 JIE5 JIE"));
    ASSERT_EQ(u8"LIŪ LIÚ LIǓ LIÙ LIU LIU", makeDisplayPinyin("LIU1 LIU2 LIU3 LIU4 LIU5 LIU"));
    ASSERT_EQ(u8"XIŌNG XIÓNG XIǑNG XIÒNG XIONG XIONG", makeDisplayPinyin("XIONG1 XIONG2 XIONG3 XIONG4 XIONG5 XIONG"));
    ASSERT_EQ(u8"HUĀ HUÁ HUǍ HUÀ HUA HUA", makeDisplayPinyin("HUA1 HUA2 HUA3 HUA4 HUA5 HUA"));
    ASSERT_EQ(u8"YUĒ YUÉ YUĚ YUÈ YUE YUE", makeDisplayPinyin("YUE1 YUE2 YUE3 YUE4 YUE5 YUE"));
    ASSERT_EQ(u8"SHUĪ SHUÍ SHUǏ SHUÌ SHUI SHUI", makeDisplayPinyin("SHUI1 SHUI2 SHUI3 SHUI4 SHUI5 SHUI"));
    ASSERT_EQ(u8"DUĪ DUÍ DUǏ DUÌ DUI DUI", makeDisplayPinyin("DUI1 DUI2 DUI3 DUI4 DUI5 DUI"));
    ASSERT_EQ(u8"DUŌ DUÓ DUǑ DUÒ DUO DUO", makeDisplayPinyin("DUO1 DUO2 DUO3 DUO4 DUO5 DUO"));
    ASSERT_EQ(u8"LÜĒ LÜĒ LÜÉ LÜÉ LÜĚ LÜĚ LÜÈ LÜÈ LÜE LÜE", makeDisplayPinyin("LVE1 LU:E1 LVE2 LU:E2 LVE3 LU:E3 LVE4 LU:E4 LVE LU:E"));
    ASSERT_EQ(u8"MĀ MÁ MǍ MÀ MA MA", makeDisplayPinyin("MA1 MA2 MA3 MA4 MA5 MA"));
    ASSERT_EQ(u8"CĒ CÉ CĚ CÈ CE CE", makeDisplayPinyin("CE1 CE2 CE3 CE4 CE5 CE"));
    ASSERT_EQ(u8"CĪ CÍ CǏ CÌ CI CI", makeDisplayPinyin("CI1 CI2 CI3 CI4 CI5 CI"));
    ASSERT_EQ(u8"BŌ BÓ BǑ BÒ BO BO", makeDisplayPinyin("BO1 BO2 BO3 BO4 BO5 BO"));
    ASSERT_EQ(u8"BŪ BÚ BǓ BÙ BU BU", makeDisplayPinyin("BU1 BU2 BU3 BU4 BU5 BU"));
    ASSERT_EQ(u8"LǕ LǕ LǗ LǗ LǙ LǙ LǛ LǛ LÜ LÜ LÜ LÜ", makeDisplayPinyin("LU:1 LV1 LU:2 LV2 LU:3 LV3 LU:4 LV4 LV5 LU:5 LV LU:"));
}

TEST(PinyinUtils, interesting) {
    ASSERT_EQ(u8"bēibéiběibèibeibei", makeToneMarkedSearchPinyin("bei1 bei2 bei3 bei4 bei5 bei"));
    ASSERT_EQ(u8"beibeibeibeibeibei", makeTonelessSearchPinyin("bei1 bei2 bei3 bei4 bei5 bei"));
    ASSERT_EQ(u8"bēibéiběibèibeibei", makeToneMarkedSearchPinyin("bei1bei2bei3bei4bei5bei"));
    ASSERT_EQ(u8"beibeibeibeibeibei", makeTonelessSearchPinyin("bei1bei2bei3bei4bei5bei"));

    ASSERT_EQ(u8"bēibéiběibèibeibei", makeToneMarkedSearchPinyin("BEI1 BEI2 BEI3 BEI4 BEI5 BEI"));
    ASSERT_EQ(u8"beibeibeibeibeibei", makeTonelessSearchPinyin("BEI1 BEI2 BEI3 BEI4 BEI5 BEI"));
    ASSERT_EQ(u8"bēibéiběibèibeibei", makeToneMarkedSearchPinyin("BEI1BEI2BEI3BEI4BEI5BEI"));
    ASSERT_EQ(u8"beibeibeibeibeibei", makeTonelessSearchPinyin("BEI1BEI2BEI3BEI4BEI5BEI"));

    ASSERT_EQ(u8"shí tou, jiǎn zi, bù", makeDisplayPinyin("shi2 tou5 , jian3 zi5 , bu4"));
    ASSERT_EQ(u8"shítoujiǎnzibù", makeToneMarkedSearchPinyin("shi2 tou5 , jian3 zi5 , bu4"));
    ASSERT_EQ(u8"shitoujianzibu", makeTonelessSearchPinyin("shi2 tou5 , jian3 zi5 , bu4"));

    ASSERT_EQ(u8"USB shǒu zhǐ", makeDisplayPinyin("U S B shou3 zhi3"));
    ASSERT_EQ(u8"usbshǒuzhǐ", makeToneMarkedSearchPinyin("U S B shou3 zhi3"));
    ASSERT_EQ(u8"usbshouzhi", makeTonelessSearchPinyin("U S B shou3 zhi3"));
    ASSERT_EQ(u8"DNA jiàn dìng", makeDisplayPinyin("D N A jian4 ding4"));
    ASSERT_EQ(u8"dnajiàndìng", makeToneMarkedSearchPinyin("D N A jian4 ding4"));
    ASSERT_EQ(u8"dnajianding", makeTonelessSearchPinyin("D N A jian4 ding4"));
    ASSERT_EQ(u8"X guāng", makeDisplayPinyin("X guang1"));
    ASSERT_EQ(u8"xguāng", makeToneMarkedSearchPinyin("X guang1"));
    ASSERT_EQ(u8"xguang", makeTonelessSearchPinyin("X guang1"));
    ASSERT_EQ(u8"VCR", makeDisplayPinyin("V C R"));
    ASSERT_EQ(u8"vcr", makeToneMarkedSearchPinyin("V C R"));
    ASSERT_EQ(u8"vcr", makeTonelessSearchPinyin("V C R"));

    ASSERT_EQ(u8"3q", makeToneMarkedSearchPinyin("3Q"));
    ASSERT_EQ(u8"3q", makeTonelessSearchPinyin("3Q"));
    ASSERT_EQ(u8"3Q", makeDisplayPinyin("3Q"));

    ASSERT_EQ(u8"Yī lóng · Mǎ sī kè", makeDisplayPinyin("Yi1 long2 · Ma3 si1 ke4"));
    ASSERT_EQ(u8"yīlóngmǎsīkè", makeToneMarkedSearchPinyin("Yi1 long2 · Ma3 si1 ke4"));
    ASSERT_EQ(u8"yilongmasike", makeTonelessSearchPinyin("Yi1 long2 · Ma3 si1 ke4"));
    ASSERT_EQ(u8"gēng", makeTonelessSearchPinyin("gēng")); //wrong, but seen in cedit

}

