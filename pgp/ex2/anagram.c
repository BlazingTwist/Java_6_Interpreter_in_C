#include "../../jvm/include/interpretedjvm.h"
#include <stdio.h>
#include <string.h>

// @formatter:off
/*
package pgp.ex2.task4;

import pgp.utils.CStringUtils;
import pgp.utils.EntryPoint;

public class Anagram {

	@EntryPoint
	public static byte[] isAnagram(byte[] params) {
		int strLen1 = CStringUtils.getCStringLength(params, 0);
		int strLen2 = CStringUtils.getCStringLength(params, strLen1 + 1);
		if (strLen1 != strLen2) {
			return new byte[]{0};
		}

		int[] charTable = new int[256];
		for (int i = 0; i < strLen1; i++) {
			charTable[0xff & params[i]]++;
		}

		int str2Offset = strLen1 + 1;
		int str2End = str2Offset + strLen2;
		for (int i = str2Offset; i < str2End; i++) {
			int charIndex = 0xff & params[i];
			int count = charTable[charIndex];
			if (count == 0) {
				return new byte[]{0};
			}
			charTable[charIndex] = count - 1;
		}
		return new byte[]{1};
	}
}
 */
// pgp/ex2/task4/Anagram
const char* file1[] = {"0000088a","cafebabe0000003200730a001500470a0048004908004a08004b0a004c004d0a004c004e09004f00500700510a000800470800520a000800530a000800540800","550a000800560a005700580a005700590a0014005a08005b0a0057005c07005d07005e0100063c696e69743e010003282956010004436f646501000f4c696e65","4e756d6265725461626c650100124c6f63616c5661726961626c655461626c65010004746869730100174c7067702f6578322f7461736b342f416e616772616d","3b0100096973416e616772616d010006285b42295b42010001690100014901000963686172496e646578010005636f756e74010006706172616d730100025b42","0100077374724c656e310100077374724c656e32010009636861725461626c650100025b4901000a737472324f666673657401000773747232456e6401000d53","7461636b4d61705461626c6507002801001b52756e74696d65496e76697369626c65416e6e6f746174696f6e730100164c7067702f7574696c732f456e747279","506f696e743b0100046d61696e010016285b4c6a6176612f6c616e672f537472696e673b29560100016301000143010004617272240100025b430100046c656e","240100026924010002623101000142010004617267730100135b4c6a6176612f6c616e672f537472696e673b010001610100124c6a6176612f6c616e672f5374","72696e673b01000162010005696e707574010005696e646578010006726573756c7407003a07005f07002407003401000a536f7572636546696c65010021416e","616772616d2e6a6176612066726f6d20496e70757446696c654f626a6563740c001600170700600c006100620100046d65686c01000468656c6d07005f0c0063","00640c006500660700670c006800690100176a6176612f6c616e672f537472696e674275696c6465720100000c006a006b0c006a006c0100022c200c006d006e","07006f0c007000710c007200170c001d001e01000c697320616e616772616d3a200c007200710100157067702f6578322f7461736b342f416e616772616d0100","106a6176612f6c616e672f4f626a6563740100106a6176612f6c616e672f537472696e670100167067702f7574696c732f43537472696e675574696c73010010","67657443537472696e674c656e677468010006285b424929490100066c656e67746801000328294901000b746f43686172417272617901000428295b43010010","6a6176612f6c616e672f53797374656d0100036f75740100154c6a6176612f696f2f5072696e7453747265616d3b010006617070656e6401002d284c6a617661","2f6c616e672f537472696e673b294c6a6176612f6c616e672f537472696e674275696c6465723b01001c2849294c6a6176612f6c616e672f537472696e674275","696c6465723b010008746f537472696e6701001428294c6a6176612f6c616e672f537472696e673b0100136a6176612f696f2f5072696e7453747265616d0100","057072696e74010015284c6a6176612f6c616e672f537472696e673b29560100077072696e746c6e002100140015000000000003000100160017000100180000","002f00010001000000052ab70001b100000002001900000006000100000006001a0000000c000100000005001b001c00000009001d001e000200180000017400","040009000000872a03b800023c2a1b0460b800023d1b1c9f000b04bc0859030354b0110100bc0a4e03360415041ba200172d1100ff2a1504337e5c2e04604f84","0401a7ffe91b0460360415041c6036051504360615061505a2002e1100ff2a1506337e36072d15072e360815089a000b04bc0859030354b02d1507150804644f","840601a7ffd104bc0859030454b00000000300190000004a00120000000a0006000b000e000c0013000d001b001000210011002a001200380011003e00150043","00160049001700540018005e00190064001a0069001b0071001d00790017007f001f001a00000066000a0024001a001f00200004005e001b0021002000070064","0015002200200008004d0032001f002000060000008700230024000000060081002500200001000e007900260020000200210066002700280003004300440029","002000040049003e002a00200005002b0000001f0006fd001b0101fd000807002c01fa0019fe000e010101fd00230101f8000d002d000000060001002e000000","09002f0030000100180000023300040009000000d912034c12044d2bb600052cb60005600560bc084e0336042bb600063a051905be360603360715071506a200","1a190515073436082d150415089154840401840701a7ffe58404012cb600063a051905be360603360715071506a2001a190515073436082d1504150891548404","01840701a7ffe52d3a051905be360603360715071506a2002f19051507333608b20007bb000859b70009120ab6000b1508b6000c120db6000bb6000eb6000f84","0701a7ffd0b20007b600102db800113a05b20007bb000859b700091212b6000b19050333b6000cb6000eb60013b1000000030019000000520014000000230003","002400060026001400270017002800330029003a002a003d00280043002c0046002d0062002e0069002f006c002d00720032008b003300aa003200b0003500b6","003700bc003800d80039001a000000b600120033000a003100320008001d0026003300340005002200210035002000060025001e0036002000070062000a0031","00320008004c0026003300340005005100210035002000060054001e003600200007008b001f0037003800080075003b003300240005007a0036003500200006","007d0033003600200007000000d90039003a0000000300d6003b003c0001000600d3003d003c0002001400c5003e00240003001700c2003f0020000400bc001d","004000240005002b000000340006ff002500080700410700420700420700430107004401010000f8001dfe00100700440101f8001dfe000a0700430101f80032","00010045000000020046"};

/*
package pgp.utils;

/ **
 * This class *really* isn't Utf-8 conform!
 * Use at your own risk.
 * /
public class CStringUtils {

	public static int getCStringLength(byte[] data, int offset) {
		int numChars = 0;
		for (int i = offset; i < data.length; i++) {
			if (data[i] == 0) {
				break;
			}
			numChars++;
		}
		return numChars;
	}

	public static String cStringToString(byte[] data, int offset) {
		return cStringToString(data, offset, getCStringLength(data, offset));
	}

	public static String cStringToString(byte[] data, int offset, int length) {
		return new String(data, 0, offset, length);
	}

	public static byte[] stringToCString(String value) {
		byte[] result = new byte[value.length() + 1];
		int index = 0;
		for (char c : value.toCharArray()) {
			result[index++] = (byte) c;
		}
		return result;
	}

	public static void writeStringToByteArray(String value, byte[] target, int offset) {
		if (value.length() + offset + 1 > target.length) {
			throw new IllegalArgumentException("'target' array is too short to hold 'value' string.");
		}
		char[] chars = value.toCharArray();
		for (int i = 0; i < chars.length; i++) {
			target[offset + i] = (byte) chars[i];
		}
		target[offset + chars.length] = 0;
	}
}
 */
// pgp/utils/CStringUtils
const char* file2[] = {"00000675","cafebabe0000003200470a000c00360a000b00370a000b00380700390a0004003a0a0004003b0a0004003c07003d08003e0a0008003f0700400700410100063c","696e69743e010003282956010004436f646501000f4c696e654e756d6265725461626c650100124c6f63616c5661726961626c655461626c6501000474686973","0100184c7067702f7574696c732f43537472696e675574696c733b01001067657443537472696e674c656e677468010006285b42492949010001690100014901","0004646174610100025b420100066f66667365740100086e756d436861727301000d537461636b4d61705461626c6501000f63537472696e67546f537472696e","67010017285b4249294c6a6176612f6c616e672f537472696e673b010018285b424949294c6a6176612f6c616e672f537472696e673b0100066c656e67746801","000f737472696e67546f43537472696e67010016284c6a6176612f6c616e672f537472696e673b295b420100016301000143010004617272240100025b430100","046c656e24010002692401000576616c75650100124c6a6176612f6c616e672f537472696e673b010006726573756c74010005696e6465780700390700190700","260100167772697465537472696e67546f427974654172726179010018284c6a6176612f6c616e672f537472696e673b5b424929560100067461726765740100","05636861727301000a536f7572636546696c6501002643537472696e675574696c732e6a6176612066726f6d20496e70757446696c654f626a6563740c000d00","0e0c001400150c001d001f0100106a6176612f6c616e672f537472696e670c000d00420c002000430c004400450100226a6176612f6c616e672f496c6c656761","6c417267756d656e74457863657074696f6e010033277461726765742720617272617920697320746f6f2073686f727420746f20686f6c64202776616c756527","20737472696e672e0c000d00460100167067702f7574696c732f43537472696e675574696c730100106a6176612f6c616e672f4f626a656374010008285b4249","4949295601000328294901000b746f43686172417272617901000428295b43010015284c6a6176612f6c616e672f537472696e673b29560021000b000c000000","0000060001000d000e0001000f0000002f00010001000000052ab70001b10000000200100000000600010000000700110000000c000100000005001200130000","0009001400150001000f0000008f000200040000001e033d1b3e1d2abea200152a1d339a0006a7000c840201840301a7ffeb1cac0000000300100000001e0007","0000000a0002000b000a000c0010000d0013000f0016000b001c001100110000002a0004000400180016001700030000001e0018001900000000001e001a0017","00010002001c001b00170002001c0000000b0003fd000401010efa00080009001d001e0001000f0000003f000400020000000b2a1b2a1bb80002b80003b00000","000200100000000600010000001500110000001600020000000b0018001900000000000b001a001700010009001d001f0001000f0000004a000600030000000c","bb0004592a031b1cb70005b00000000200100000000600010000001900110000002000030000000c0018001900000000000c001a001700010000000c00200017","00020009002100220001000f000000cd00030007000000352ab600060460bc084c033d2ab600074e2dbe360403360515051504a200182d15053436062b1c8402","0115069154840501a7ffe72bb00000000300100000001a00060000001d0009001e000b001f00240020002d001f00330022001100000048000700240009002300","240006001000230025002600030014001f0027001700040017001c002800170005000000350029002a00000009002c002b00190001000b002a002c0017000200","1c000000180002ff0017000607002d07002e0107002f01010000f8001b0009003000310001000f000000c0000400050000003f2ab600061c6004602bbea4000d","bb0008591209b7000abf2ab600074e03360415042dbea200142b1c1504602d1504349154840401a7ffeb2b1c2dbe600354b10000000300100000002200080000","0026000d002700170029001c002a0026002b0031002a0037002d003e002e0011000000340005001f00180016001700040000003f0029002a00000000003f0032","001900010000003f001a00170002001c0023003300260003001c0000000d000317fd000707002f01fa001700010034000000020035"};

// @formatter:on

const char **files[] = {file1, file2};
int numFiles = sizeof(files) / sizeof(files[0]);
InterpretedJVM jvm;

int is_anagram(char *word, char *compared) {
    int wordLen = strlen(word);
    int comparedLen = strlen(compared);
    int numBytes = wordLen + comparedLen + 2;

    HeapPointer mainArg = createPrimitiveArray_4Byte(numBytes);
    for (int i = 0; i <= wordLen; ++i) {
        mainArg->data[i] = (U1) word[i];
    }
    for (int i = 0; i <= comparedLen; ++i) {
        mainArg->data[i + wordLen + 1] = (U1) compared[i];
    }

    push_Object(&jvm, mainArg);
    mainArg = NULL;
    push_Method(&jvm,
                "pgp/ex2/task4/Anagram",
                "isAnagram",
                "([B)[B"
    );
    HeapPointer returnValue = executeAllMethods_SingleReturn(&jvm);

    assertObjectType(OBJ_PrimitiveArray, returnValue); // should be byte[]
    int result = (int) ((I1) returnValue->data[0]);
    decrementReferenceCount(returnValue);

    return result;
}

int main() {
    jvm = prepareJVM(files, numFiles);

    printf("%d\n", is_anagram("mehl", "helm"));
    printf("%d\n", is_anagram("boeing", "airbus"));
    printf("%d\n", is_anagram("", ""));


    releaseJVM(&jvm);
}