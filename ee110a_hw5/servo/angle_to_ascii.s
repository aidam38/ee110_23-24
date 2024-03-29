;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                              angle_to_ascii.s                              ;
;                         Angle to ASCII conversion                          ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains a function which converts an angle in the range 
; [-359, 359] to an ASCII string. This function takes a brute-force 
; approach by defining a huge table of strings, because implementing
; generic integer to string conversion would be duplicate the C standard
; library. This file was created as a quick-and-dirty solution for printing
; an angle value on a display.
; 
; Revision History: 
;		12/5/23	Adam Krivka		initial revision

; local includes
; none

; export function
	.def AngleToAscii

; self-contained defines
MIN_ANGLE .equ		359	; minimum angle value we can print
MAX_ANGLE .equ		359		; maximum angle value we can print
ANGLE_RANGE .equ	(MAX_ANGLE - MIN_ANGLE)

; strings for numbers between -360 and 360
; GENERATED BY CHATGPT
sm359: .cstring "-359"
sm358: .cstring "-358"
sm357: .cstring "-357"
sm356: .cstring "-356"
sm355: .cstring "-355"
sm354: .cstring "-354"
sm353: .cstring "-353"
sm352: .cstring "-352"
sm351: .cstring "-351"
sm350: .cstring "-350"
sm349: .cstring "-349"
sm348: .cstring "-348"
sm347: .cstring "-347"
sm346: .cstring "-346"
sm345: .cstring "-345"
sm344: .cstring "-344"
sm343: .cstring "-343"
sm342: .cstring "-342"
sm341: .cstring "-341"
sm340: .cstring "-340"
sm339: .cstring "-339"
sm338: .cstring "-338"
sm337: .cstring "-337"
sm336: .cstring "-336"
sm335: .cstring "-335"
sm334: .cstring "-334"
sm333: .cstring "-333"
sm332: .cstring "-332"
sm331: .cstring "-331"
sm330: .cstring "-330"
sm329: .cstring "-329"
sm328: .cstring "-328"
sm327: .cstring "-327"
sm326: .cstring "-326"
sm325: .cstring "-325"
sm324: .cstring "-324"
sm323: .cstring "-323"
sm322: .cstring "-322"
sm321: .cstring "-321"
sm320: .cstring "-320"
sm319: .cstring "-319"
sm318: .cstring "-318"
sm317: .cstring "-317"
sm316: .cstring "-316"
sm315: .cstring "-315"
sm314: .cstring "-314"
sm313: .cstring "-313"
sm312: .cstring "-312"
sm311: .cstring "-311"
sm310: .cstring "-310"
sm309: .cstring "-309"
sm308: .cstring "-308"
sm307: .cstring "-307"
sm306: .cstring "-306"
sm305: .cstring "-305"
sm304: .cstring "-304"
sm303: .cstring "-303"
sm302: .cstring "-302"
sm301: .cstring "-301"
sm300: .cstring "-300"
sm299: .cstring "-299"
sm298: .cstring "-298"
sm297: .cstring "-297"
sm296: .cstring "-296"
sm295: .cstring "-295"
sm294: .cstring "-294"
sm293: .cstring "-293"
sm292: .cstring "-292"
sm291: .cstring "-291"
sm290: .cstring "-290"
sm289: .cstring "-289"
sm288: .cstring "-288"
sm287: .cstring "-287"
sm286: .cstring "-286"
sm285: .cstring "-285"
sm284: .cstring "-284"
sm283: .cstring "-283"
sm282: .cstring "-282"
sm281: .cstring "-281"
sm280: .cstring "-280"
sm279: .cstring "-279"
sm278: .cstring "-278"
sm277: .cstring "-277"
sm276: .cstring "-276"
sm275: .cstring "-275"
sm274: .cstring "-274"
sm273: .cstring "-273"
sm272: .cstring "-272"
sm271: .cstring "-271"
sm270: .cstring "-270"
sm269: .cstring "-269"
sm268: .cstring "-268"
sm267: .cstring "-267"
sm266: .cstring "-266"
sm265: .cstring "-265"
sm264: .cstring "-264"
sm263: .cstring "-263"
sm262: .cstring "-262"
sm261: .cstring "-261"
sm260: .cstring "-260"
sm259: .cstring "-259"
sm258: .cstring "-258"
sm257: .cstring "-257"
sm256: .cstring "-256"
sm255: .cstring "-255"
sm254: .cstring "-254"
sm253: .cstring "-253"
sm252: .cstring "-252"
sm251: .cstring "-251"
sm250: .cstring "-250"
sm249: .cstring "-249"
sm248: .cstring "-248"
sm247: .cstring "-247"
sm246: .cstring "-246"
sm245: .cstring "-245"
sm244: .cstring "-244"
sm243: .cstring "-243"
sm242: .cstring "-242"
sm241: .cstring "-241"
sm240: .cstring "-240"
sm239: .cstring "-239"
sm238: .cstring "-238"
sm237: .cstring "-237"
sm236: .cstring "-236"
sm235: .cstring "-235"
sm234: .cstring "-234"
sm233: .cstring "-233"
sm232: .cstring "-232"
sm231: .cstring "-231"
sm230: .cstring "-230"
sm229: .cstring "-229"
sm228: .cstring "-228"
sm227: .cstring "-227"
sm226: .cstring "-226"
sm225: .cstring "-225"
sm224: .cstring "-224"
sm223: .cstring "-223"
sm222: .cstring "-222"
sm221: .cstring "-221"
sm220: .cstring "-220"
sm219: .cstring "-219"
sm218: .cstring "-218"
sm217: .cstring "-217"
sm216: .cstring "-216"
sm215: .cstring "-215"
sm214: .cstring "-214"
sm213: .cstring "-213"
sm212: .cstring "-212"
sm211: .cstring "-211"
sm210: .cstring "-210"
sm209: .cstring "-209"
sm208: .cstring "-208"
sm207: .cstring "-207"
sm206: .cstring "-206"
sm205: .cstring "-205"
sm204: .cstring "-204"
sm203: .cstring "-203"
sm202: .cstring "-202"
sm201: .cstring "-201"
sm200: .cstring "-200"
sm199: .cstring "-199"
sm198: .cstring "-198"
sm197: .cstring "-197"
sm196: .cstring "-196"
sm195: .cstring "-195"
sm194: .cstring "-194"
sm193: .cstring "-193"
sm192: .cstring "-192"
sm191: .cstring "-191"
sm190: .cstring "-190"
sm189: .cstring "-189"
sm188: .cstring "-188"
sm187: .cstring "-187"
sm186: .cstring "-186"
sm185: .cstring "-185"
sm184: .cstring "-184"
sm183: .cstring "-183"
sm182: .cstring "-182"
sm181: .cstring "-181"
sm180: .cstring "-180"
sm179: .cstring "-179"
sm178: .cstring "-178"
sm177: .cstring "-177"
sm176: .cstring "-176"
sm175: .cstring "-175"
sm174: .cstring "-174"
sm173: .cstring "-173"
sm172: .cstring "-172"
sm171: .cstring "-171"
sm170: .cstring "-170"
sm169: .cstring "-169"
sm168: .cstring "-168"
sm167: .cstring "-167"
sm166: .cstring "-166"
sm165: .cstring "-165"
sm164: .cstring "-164"
sm163: .cstring "-163"
sm162: .cstring "-162"
sm161: .cstring "-161"
sm160: .cstring "-160"
sm159: .cstring "-159"
sm158: .cstring "-158"
sm157: .cstring "-157"
sm156: .cstring "-156"
sm155: .cstring "-155"
sm154: .cstring "-154"
sm153: .cstring "-153"
sm152: .cstring "-152"
sm151: .cstring "-151"
sm150: .cstring "-150"
sm149: .cstring "-149"
sm148: .cstring "-148"
sm147: .cstring "-147"
sm146: .cstring "-146"
sm145: .cstring "-145"
sm144: .cstring "-144"
sm143: .cstring "-143"
sm142: .cstring "-142"
sm141: .cstring "-141"
sm140: .cstring "-140"
sm139: .cstring "-139"
sm138: .cstring "-138"
sm137: .cstring "-137"
sm136: .cstring "-136"
sm135: .cstring "-135"
sm134: .cstring "-134"
sm133: .cstring "-133"
sm132: .cstring "-132"
sm131: .cstring "-131"
sm130: .cstring "-130"
sm129: .cstring "-129"
sm128: .cstring "-128"
sm127: .cstring "-127"
sm126: .cstring "-126"
sm125: .cstring "-125"
sm124: .cstring "-124"
sm123: .cstring "-123"
sm122: .cstring "-122"
sm121: .cstring "-121"
sm120: .cstring "-120"
sm119: .cstring "-119"
sm118: .cstring "-118"
sm117: .cstring "-117"
sm116: .cstring "-116"
sm115: .cstring "-115"
sm114: .cstring "-114"
sm113: .cstring "-113"
sm112: .cstring "-112"
sm111: .cstring "-111"
sm110: .cstring "-110"
sm109: .cstring "-109"
sm108: .cstring "-108"
sm107: .cstring "-107"
sm106: .cstring "-106"
sm105: .cstring "-105"
sm104: .cstring "-104"
sm103: .cstring "-103"
sm102: .cstring "-102"
sm101: .cstring "-101"
sm100: .cstring "-100"
sm99: .cstring "-99 "
sm98: .cstring "-98 "
sm97: .cstring "-97 "
sm96: .cstring "-96 "
sm95: .cstring "-95 "
sm94: .cstring "-94 "
sm93: .cstring "-93 "
sm92: .cstring "-92 "
sm91: .cstring "-91 "
sm90: .cstring "-90 "
sm89: .cstring "-89 "
sm88: .cstring "-88 "
sm87: .cstring "-87 "
sm86: .cstring "-86 "
sm85: .cstring "-85 "
sm84: .cstring "-84 "
sm83: .cstring "-83 "
sm82: .cstring "-82 "
sm81: .cstring "-81 "
sm80: .cstring "-80 "
sm79: .cstring "-79 "
sm78: .cstring "-78 "
sm77: .cstring "-77 "
sm76: .cstring "-76 "
sm75: .cstring "-75 "
sm74: .cstring "-74 "
sm73: .cstring "-73 "
sm72: .cstring "-72 "
sm71: .cstring "-71 "
sm70: .cstring "-70 "
sm69: .cstring "-69 "
sm68: .cstring "-68 "
sm67: .cstring "-67 "
sm66: .cstring "-66 "
sm65: .cstring "-65 "
sm64: .cstring "-64 "
sm63: .cstring "-63 "
sm62: .cstring "-62 "
sm61: .cstring "-61 "
sm60: .cstring "-60 "
sm59: .cstring "-59 "
sm58: .cstring "-58 "
sm57: .cstring "-57 "
sm56: .cstring "-56 "
sm55: .cstring "-55 "
sm54: .cstring "-54 "
sm53: .cstring "-53 "
sm52: .cstring "-52 "
sm51: .cstring "-51 "
sm50: .cstring "-50 "
sm49: .cstring "-49 "
sm48: .cstring "-48 "
sm47: .cstring "-47 "
sm46: .cstring "-46 "
sm45: .cstring "-45 "
sm44: .cstring "-44 "
sm43: .cstring "-43 "
sm42: .cstring "-42 "
sm41: .cstring "-41 "
sm40: .cstring "-40 "
sm39: .cstring "-39 "
sm38: .cstring "-38 "
sm37: .cstring "-37 "
sm36: .cstring "-36 "
sm35: .cstring "-35 "
sm34: .cstring "-34 "
sm33: .cstring "-33 "
sm32: .cstring "-32 "
sm31: .cstring "-31 "
sm30: .cstring "-30 "
sm29: .cstring "-29 "
sm28: .cstring "-28 "
sm27: .cstring "-27 "
sm26: .cstring "-26 "
sm25: .cstring "-25 "
sm24: .cstring "-24 "
sm23: .cstring "-23 "
sm22: .cstring "-22 "
sm21: .cstring "-21 "
sm20: .cstring "-20 "
sm19: .cstring "-19 "
sm18: .cstring "-18 "
sm17: .cstring "-17 "
sm16: .cstring "-16 "
sm15: .cstring "-15 "
sm14: .cstring "-14 "
sm13: .cstring "-13 "
sm12: .cstring "-12 "
sm11: .cstring "-11 "
sm10: .cstring "-10 "
sm9: .cstring "-9  "
sm8: .cstring "-8  "
sm7: .cstring "-7  "
sm6: .cstring "-6  "
sm5: .cstring "-5  "
sm4: .cstring "-4  "
sm3: .cstring "-3  "
sm2: .cstring "-2  "
sm1: .cstring "-1  "
ss0: .cstring "0   "
ss1: .cstring "1   "
ss2: .cstring "2   "
ss3: .cstring "3   "
ss4: .cstring "4   "
ss5: .cstring "5   "
ss6: .cstring "6   "
ss7: .cstring "7   "
ss8: .cstring "8   "
ss9: .cstring "9   "
ss10: .cstring "10  "
ss11: .cstring "11  "
ss12: .cstring "12  "
ss13: .cstring "13  "
ss14: .cstring "14  "
ss15: .cstring "15  "
ss16: .cstring "16  "
ss17: .cstring "17  "
ss18: .cstring "18  "
ss19: .cstring "19  "
ss20: .cstring "20  "
ss21: .cstring "21  "
ss22: .cstring "22  "
ss23: .cstring "23  "
ss24: .cstring "24  "
ss25: .cstring "25  "
ss26: .cstring "26  "
ss27: .cstring "27  "
ss28: .cstring "28  "
ss29: .cstring "29  "
ss30: .cstring "30  "
ss31: .cstring "31  "
ss32: .cstring "32  "
ss33: .cstring "33  "
ss34: .cstring "34  "
ss35: .cstring "35  "
ss36: .cstring "36  "
ss37: .cstring "37  "
ss38: .cstring "38  "
ss39: .cstring "39  "
ss40: .cstring "40  "
ss41: .cstring "41  "
ss42: .cstring "42  "
ss43: .cstring "43  "
ss44: .cstring "44  "
ss45: .cstring "45  "
ss46: .cstring "46  "
ss47: .cstring "47  "
ss48: .cstring "48  "
ss49: .cstring "49  "
ss50: .cstring "50  "
ss51: .cstring "51  "
ss52: .cstring "52  "
ss53: .cstring "53  "
ss54: .cstring "54  "
ss55: .cstring "55  "
ss56: .cstring "56  "
ss57: .cstring "57  "
ss58: .cstring "58  "
ss59: .cstring "59  "
ss60: .cstring "60  "
ss61: .cstring "61  "
ss62: .cstring "62  "
ss63: .cstring "63  "
ss64: .cstring "64  "
ss65: .cstring "65  "
ss66: .cstring "66  "
ss67: .cstring "67  "
ss68: .cstring "68  "
ss69: .cstring "69  "
ss70: .cstring "70  "
ss71: .cstring "71  "
ss72: .cstring "72  "
ss73: .cstring "73  "
ss74: .cstring "74  "
ss75: .cstring "75  "
ss76: .cstring "76  "
ss77: .cstring "77  "
ss78: .cstring "78  "
ss79: .cstring "79  "
ss80: .cstring "80  "
ss81: .cstring "81  "
ss82: .cstring "82  "
ss83: .cstring "83  "
ss84: .cstring "84  "
ss85: .cstring "85  "
ss86: .cstring "86  "
ss87: .cstring "87  "
ss88: .cstring "88  "
ss89: .cstring "89  "
ss90: .cstring "90  "
ss91: .cstring "91  "
ss92: .cstring "92  "
ss93: .cstring "93  "
ss94: .cstring "94  "
ss95: .cstring "95  "
ss96: .cstring "96  "
ss97: .cstring "97  "
ss98: .cstring "98  "
ss99: .cstring "99  "
ss100: .cstring "100 "
ss101: .cstring "101 "
ss102: .cstring "102 "
ss103: .cstring "103 "
ss104: .cstring "104 "
ss105: .cstring "105 "
ss106: .cstring "106 "
ss107: .cstring "107 "
ss108: .cstring "108 "
ss109: .cstring "109 "
ss110: .cstring "110 "
ss111: .cstring "111 "
ss112: .cstring "112 "
ss113: .cstring "113 "
ss114: .cstring "114 "
ss115: .cstring "115 "
ss116: .cstring "116 "
ss117: .cstring "117 "
ss118: .cstring "118 "
ss119: .cstring "119 "
ss120: .cstring "120 "
ss121: .cstring "121 "
ss122: .cstring "122 "
ss123: .cstring "123 "
ss124: .cstring "124 "
ss125: .cstring "125 "
ss126: .cstring "126 "
ss127: .cstring "127 "
ss128: .cstring "128 "
ss129: .cstring "129 "
ss130: .cstring "130 "
ss131: .cstring "131 "
ss132: .cstring "132 "
ss133: .cstring "133 "
ss134: .cstring "134 "
ss135: .cstring "135 "
ss136: .cstring "136 "
ss137: .cstring "137 "
ss138: .cstring "138 "
ss139: .cstring "139 "
ss140: .cstring "140 "
ss141: .cstring "141 "
ss142: .cstring "142 "
ss143: .cstring "143 "
ss144: .cstring "144 "
ss145: .cstring "145 "
ss146: .cstring "146 "
ss147: .cstring "147 "
ss148: .cstring "148 "
ss149: .cstring "149 "
ss150: .cstring "150 "
ss151: .cstring "151 "
ss152: .cstring "152 "
ss153: .cstring "153 "
ss154: .cstring "154 "
ss155: .cstring "155 "
ss156: .cstring "156 "
ss157: .cstring "157 "
ss158: .cstring "158 "
ss159: .cstring "159 "
ss160: .cstring "160 "
ss161: .cstring "161 "
ss162: .cstring "162 "
ss163: .cstring "163 "
ss164: .cstring "164 "
ss165: .cstring "165 "
ss166: .cstring "166 "
ss167: .cstring "167 "
ss168: .cstring "168 "
ss169: .cstring "169 "
ss170: .cstring "170 "
ss171: .cstring "171 "
ss172: .cstring "172 "
ss173: .cstring "173 "
ss174: .cstring "174 "
ss175: .cstring "175 "
ss176: .cstring "176 "
ss177: .cstring "177 "
ss178: .cstring "178 "
ss179: .cstring "179 "
ss180: .cstring "180 "
ss181: .cstring "181 "
ss182: .cstring "182 "
ss183: .cstring "183 "
ss184: .cstring "184 "
ss185: .cstring "185 "
ss186: .cstring "186 "
ss187: .cstring "187 "
ss188: .cstring "188 "
ss189: .cstring "189 "
ss190: .cstring "190 "
ss191: .cstring "191 "
ss192: .cstring "192 "
ss193: .cstring "193 "
ss194: .cstring "194 "
ss195: .cstring "195 "
ss196: .cstring "196 "
ss197: .cstring "197 "
ss198: .cstring "198 "
ss199: .cstring "199 "
ss200: .cstring "200 "
ss201: .cstring "201 "
ss202: .cstring "202 "
ss203: .cstring "203 "
ss204: .cstring "204 "
ss205: .cstring "205 "
ss206: .cstring "206 "
ss207: .cstring "207 "
ss208: .cstring "208 "
ss209: .cstring "209 "
ss210: .cstring "210 "
ss211: .cstring "211 "
ss212: .cstring "212 "
ss213: .cstring "213 "
ss214: .cstring "214 "
ss215: .cstring "215 "
ss216: .cstring "216 "
ss217: .cstring "217 "
ss218: .cstring "218 "
ss219: .cstring "219 "
ss220: .cstring "220 "
ss221: .cstring "221 "
ss222: .cstring "222 "
ss223: .cstring "223 "
ss224: .cstring "224 "
ss225: .cstring "225 "
ss226: .cstring "226 "
ss227: .cstring "227 "
ss228: .cstring "228 "
ss229: .cstring "229 "
ss230: .cstring "230 "
ss231: .cstring "231 "
ss232: .cstring "232 "
ss233: .cstring "233 "
ss234: .cstring "234 "
ss235: .cstring "235 "
ss236: .cstring "236 "
ss237: .cstring "237 "
ss238: .cstring "238 "
ss239: .cstring "239 "
ss240: .cstring "240 "
ss241: .cstring "241 "
ss242: .cstring "242 "
ss243: .cstring "243 "
ss244: .cstring "244 "
ss245: .cstring "245 "
ss246: .cstring "246 "
ss247: .cstring "247 "
ss248: .cstring "248 "
ss249: .cstring "249 "
ss250: .cstring "250 "
ss251: .cstring "251 "
ss252: .cstring "252 "
ss253: .cstring "253 "
ss254: .cstring "254 "
ss255: .cstring "255 "
ss256: .cstring "256 "
ss257: .cstring "257 "
ss258: .cstring "258 "
ss259: .cstring "259 "
ss260: .cstring "260 "
ss261: .cstring "261 "
ss262: .cstring "262 "
ss263: .cstring "263 "
ss264: .cstring "264 "
ss265: .cstring "265 "
ss266: .cstring "266 "
ss267: .cstring "267 "
ss268: .cstring "268 "
ss269: .cstring "269 "
ss270: .cstring "270 "
ss271: .cstring "271 "
ss272: .cstring "272 "
ss273: .cstring "273 "
ss274: .cstring "274 "
ss275: .cstring "275 "
ss276: .cstring "276 "
ss277: .cstring "277 "
ss278: .cstring "278 "
ss279: .cstring "279 "
ss280: .cstring "280 "
ss281: .cstring "281 "
ss282: .cstring "282 "
ss283: .cstring "283 "
ss284: .cstring "284 "
ss285: .cstring "285 "
ss286: .cstring "286 "
ss287: .cstring "287 "
ss288: .cstring "288 "
ss289: .cstring "289 "
ss290: .cstring "290 "
ss291: .cstring "291 "
ss292: .cstring "292 "
ss293: .cstring "293 "
ss294: .cstring "294 "
ss295: .cstring "295 "
ss296: .cstring "296 "
ss297: .cstring "297 "
ss298: .cstring "298 "
ss299: .cstring "299 "
ss300: .cstring "300 "
ss301: .cstring "301 "
ss302: .cstring "302 "
ss303: .cstring "303 "
ss304: .cstring "304 "
ss305: .cstring "305 "
ss306: .cstring "306 "
ss307: .cstring "307 "
ss308: .cstring "308 "
ss309: .cstring "309 "
ss310: .cstring "310 "
ss311: .cstring "311 "
ss312: .cstring "312 "
ss313: .cstring "313 "
ss314: .cstring "314 "
ss315: .cstring "315 "
ss316: .cstring "316 "
ss317: .cstring "317 "
ss318: .cstring "318 "
ss319: .cstring "319 "
ss320: .cstring "320 "
ss321: .cstring "321 "
ss322: .cstring "322 "
ss323: .cstring "323 "
ss324: .cstring "324 "
ss325: .cstring "325 "
ss326: .cstring "326 "
ss327: .cstring "327 "
ss328: .cstring "328 "
ss329: .cstring "329 "
ss330: .cstring "330 "
ss331: .cstring "331 "
ss332: .cstring "332 "
ss333: .cstring "333 "
ss334: .cstring "334 "
ss335: .cstring "335 "
ss336: .cstring "336 "
ss337: .cstring "337 "
ss338: .cstring "338 "
ss339: .cstring "339 "
ss340: .cstring "340 "
ss341: .cstring "341 "
ss342: .cstring "342 "
ss343: .cstring "343 "
ss344: .cstring "344 "
ss345: .cstring "345 "
ss346: .cstring "346 "
ss347: .cstring "347 "
ss348: .cstring "348 "
ss349: .cstring "349 "
ss350: .cstring "350 "
ss351: .cstring "351 "
ss352: .cstring "352 "
ss353: .cstring "353 "
ss354: .cstring "354 "
ss355: .cstring "355 "
ss356: .cstring "356 "
ss357: .cstring "357 "
ss358: .cstring "358 "
ss359: .cstring "359 "

	.align 4
AngleToAsciiTab:
	.word sm359, sm358, sm357, sm356, sm355, sm354, sm353, sm352, sm351, sm350, sm349, sm348, sm347, sm346, sm345, sm344, sm343, sm342, sm341, sm340, sm339, sm338, sm337, sm336, sm335, sm334, sm333, sm332, sm331, sm330, sm329, sm328, sm327, sm326, sm325, sm324, sm323, sm322, sm321, sm320, sm319, sm318, sm317, sm316, sm315, sm314, sm313, sm312, sm311, sm310, sm309, sm308, sm307, sm306, sm305, sm304, sm303, sm302, sm301, sm300, sm299, sm298, sm297, sm296, sm295, sm294, sm293, sm292, sm291, sm290, sm289, sm288, sm287, sm286, sm285, sm284, sm283, sm282, sm281, sm280, sm279, sm278, sm277, sm276, sm275, sm274, sm273, sm272, sm271, sm270, sm269, sm268, sm267, sm266, sm265, sm264, sm263, sm262, sm261, sm260, sm259, sm258, sm257, sm256, sm255, sm254, sm253, sm252, sm251, sm250, sm249, sm248, sm247, sm246, sm245, sm244, sm243, sm242, sm241, sm240, sm239, sm238, sm237, sm236, sm235, sm234, sm233, sm232, sm231, sm230, sm229, sm228, sm227, sm226, sm225, sm224, sm223, sm222, sm221, sm220, sm219, sm218, sm217, sm216, sm215, sm214, sm213, sm212, sm211, sm210, sm209, sm208, sm207, sm206, sm205, sm204, sm203, sm202, sm201, sm200, sm199, sm198, sm197, sm196, sm195, sm194, sm193, sm192, sm191, sm190, sm189, sm188, sm187, sm186, sm185, sm184, sm183, sm182, sm181, sm180, sm179, sm178, sm177, sm176, sm175, sm174, sm173, sm172, sm171, sm170, sm169, sm168, sm167, sm166, sm165, sm164, sm163, sm162, sm161, sm160, sm159, sm158, sm157, sm156, sm155, sm154, sm153, sm152, sm151, sm150, sm149, sm148, sm147, sm146, sm145, sm144, sm143, sm142, sm141, sm140, sm139, sm138, sm137, sm136, sm135, sm134, sm133, sm132, sm131, sm130, sm129, sm128, sm127, sm126, sm125, sm124, sm123, sm122, sm121, sm120, sm119, sm118, sm117, sm116, sm115, sm114, sm113, sm112, sm111, sm110, sm109, sm108, sm107, sm106, sm105, sm104, sm103, sm102, sm101, sm100, sm99, sm98, sm97, sm96, sm95, sm94, sm93, sm92, sm91, sm90, sm89, sm88, sm87, sm86, sm85, sm84, sm83, sm82, sm81, sm80, sm79, sm78, sm77, sm76, sm75, sm74, sm73, sm72, sm71, sm70, sm69, sm68, sm67, sm66, sm65, sm64, sm63, sm62, sm61, sm60, sm59, sm58, sm57, sm56, sm55, sm54, sm53, sm52, sm51, sm50, sm49, sm48, sm47, sm46, sm45, sm44, sm43, sm42, sm41, sm40, sm39, sm38, sm37, sm36, sm35, sm34, sm33, sm32, sm31, sm30, sm29, sm28, sm27, sm26, sm25, sm24, sm23, sm22, sm21, sm20, sm19, sm18, sm17, sm16, sm15, sm14, sm13, sm12, sm11, sm10, sm9, sm8, sm7, sm6, sm5, sm4, sm3, sm2, sm1, ss0, ss1, ss2, ss3, ss4, ss5, ss6, ss7, ss8, ss9, ss10, ss11, ss12, ss13, ss14, ss15, ss16, ss17, ss18, ss19, ss20, ss21, ss22, ss23, ss24, ss25, ss26, ss27, ss28, ss29, ss30, ss31, ss32, ss33, ss34, ss35, ss36, ss37, ss38, ss39, ss40, ss41, ss42, ss43, ss44, ss45, ss46, ss47, ss48, ss49, ss50, ss51, ss52, ss53, ss54, ss55, ss56, ss57, ss58, ss59, ss60, ss61, ss62, ss63, ss64, ss65, ss66, ss67, ss68, ss69, ss70, ss71, ss72, ss73, ss74, ss75, ss76, ss77, ss78, ss79, ss80, ss81, ss82, ss83, ss84, ss85, ss86, ss87, ss88, ss89, ss90, ss91, ss92, ss93, ss94, ss95, ss96, ss97, ss98, ss99, ss100, ss101, ss102, ss103, ss104, ss105, ss106, ss107, ss108, ss109, ss110, ss111, ss112, ss113, ss114, ss115, ss116, ss117, ss118, ss119, ss120, ss121, ss122, ss123, ss124, ss125, ss126, ss127, ss128, ss129, ss130, ss131, ss132, ss133, ss134, ss135, ss136, ss137, ss138, ss139, ss140, ss141, ss142, ss143, ss144, ss145, ss146, ss147, ss148, ss149, ss150, ss151, ss152, ss153, ss154, ss155, ss156, ss157, ss158, ss159, ss160, ss161, ss162, ss163, ss164, ss165, ss166, ss167, ss168, ss169, ss170, ss171, ss172, ss173, ss174, ss175, ss176, ss177, ss178, ss179, ss180, ss181, ss182, ss183, ss184, ss185, ss186, ss187, ss188, ss189, ss190, ss191, ss192, ss193, ss194, ss195, ss196, ss197, ss198, ss199, ss200, ss201, ss202, ss203, ss204, ss205, ss206, ss207, ss208, ss209, ss210, ss211, ss212, ss213, ss214, ss215, ss216, ss217, ss218, ss219, ss220, ss221, ss222, ss223, ss224, ss225, ss226, ss227, ss228, ss229, ss230, ss231, ss232, ss233, ss234, ss235, ss236, ss237, ss238, ss239, ss240, ss241, ss242, ss243, ss244, ss245, ss246, ss247, ss248, ss249, ss250, ss251, ss252, ss253, ss254, ss255, ss256, ss257, ss258, ss259, ss260, ss261, ss262, ss263, ss264, ss265, ss266, ss267, ss268, ss269, ss270, ss271, ss272, ss273, ss274, ss275, ss276, ss277, ss278, ss279, ss280, ss281, ss282, ss283, ss284, ss285, ss286, ss287, ss288, ss289, ss290, ss291, ss292, ss293, ss294, ss295, ss296, ss297, ss298, ss299, ss300, ss301, ss302, ss303, ss304, ss305, ss306, ss307, ss308, ss309, ss310, ss311, ss312, ss313, ss314, ss315, ss316, ss317, ss318, ss319, ss320, ss321, ss322, ss323, ss324, ss325, ss326, ss327, ss328, ss329, ss330, ss331, ss332, ss333, ss334, ss335, ss336, ss337, ss338, ss339, ss340, ss341, ss342, ss343, ss344, ss345, ss346, ss347, ss348, ss349, ss350, ss351, ss352, ss353, ss354, ss355, ss356, ss357, ss358, ss359

; AngleToAscii
;
; Description:          Converts angle in range [-359, 359] to an ASCII string
;						pointer.
;
; Arguments:            angle in R0.
; Return Values:        string pointer in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
; 
; Revision History:
;		12/5/23	Adam Krivka		initial revision

AngleToAscii:
	PUSH	{LR}			; save return address and used registers

	MOV		R1, #MIN_ANGLE	; [MIN_ANGLE, MAX_ANGLE] => [0, ANGLE_RANGE]
	ADD		R1, R0

	ADR		R2, AngleToAsciiTab ; prepare tab base address
	LDR		R0, [R2, R1, LSL #2]; look up string, return it in R0

	POP		{LR}			; restore return address
	BX		LR				; return

