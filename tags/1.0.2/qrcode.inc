
<!-- saved from url=(0068)http://d-project.googlecode.com/svn/trunk/misc/qrcode/php/qrcode.php -->
<html><head><meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1"><style type="text/css"></style></head><body><pre style="word-wrap: break-word; white-space: pre-wrap;">&lt;?php

//---------------------------------------------------------------
// QRCode for PHP4
//
// Copyright (c) 2009 Kazuhiko Arase
//
// URL: http://www.d-project.com/
//
// Licensed under the MIT license:
//   http://www.opensource.org/licenses/mit-license.php
//
// The word "QR Code" is registered trademark of 
// DENSO WAVE INCORPORATED
//   http://www.denso-wave.com/qrcode/faqpatent-e.html
//
//---------------------------------------------------------------------

//---------------------------------------------------------------
// QRCode
//---------------------------------------------------------------

define("QR_PAD0", 0xEC);
define("QR_PAD1", 0x11);

class QRCode {

	var $typeNumber;

	var $modules;

	var $moduleCount;

	var $errorCorrectLevel;

	var $qrDataList;

	function QRCode() {
 		$this-&gt;typeNumber = 1;
		$this-&gt;errorCorrectLevel = QR_ERROR_CORRECT_LEVEL_H;
		$this-&gt;qrDataList = array();
	}

	function getTypeNumber() {
		return $this-&gt;typeNumber;
	}

	function setTypeNumber($typeNumber) {
		$this-&gt;typeNumber = $typeNumber;
	}

    function getErrorCorrectLevel() {
        return $this-&gt;errorCorrectLevel;
    }

    function setErrorCorrectLevel($errorCorrectLevel) {
        $this-&gt;errorCorrectLevel = $errorCorrectLevel;
    }

	function addData($data, $mode = 0) {

		if ($mode == 0) {
			$mode = QRUtil::getMode($data);
		}
		
		switch($mode) {

		case QR_MODE_NUMBER :
			$this-&gt;addDataImpl(new QRNumber($data) );
			break;

		case QR_MODE_ALPHA_NUM :
			$this-&gt;addDataImpl(new QRAlphaNum($data) );
			break;

		case QR_MODE_8BIT_BYTE :
			$this-&gt;addDataImpl(new QR8BitByte($data) );
			break;

		case QR_MODE_KANJI :
			$this-&gt;addDataImpl(new QRKanji($data) );
			break;

		default :
			trigger_error("mode:$mode", E_USER_ERROR);
		}
	}

	function clearData() {
		$qrDataList = array();
	}
	
	function addDataImpl(&amp;$qrData) {
		$this-&gt;qrDataList[] = $qrData;
	}

	function getDataCount() {
		return count($this-&gt;qrDataList);
	}
	
	function getData($index) {
		return $this-&gt;qrDataList[$index];
	}

	function isDark($row, $col) {
		if ($this-&gt;modules[$row][$col] !== null) {
			return $this-&gt;modules[$row][$col];
		} else {
			return false;
		}
	}

	function getModuleCount() {
		return $this-&gt;moduleCount;
	}

	function make() {
		$this-&gt;makeImpl(false, $this-&gt;getBestMaskPattern() );
	}

	function getBestMaskPattern() {

		$minLostPoint = 0;
		$pattern = 0;

		for ($i = 0; $i &lt; 8; $i++) {

			$this-&gt;makeImpl(true, $i);

			$lostPoint = QRUtil::getLostPoint($this);

			if ($i == 0 || $minLostPoint &gt; $lostPoint) {
				$minLostPoint = $lostPoint;
				$pattern = $i;
			}
		}

		return $pattern;
	}
	
	function createNullArray($length) {
		$nullArray = array();
		for ($i = 0; $i &lt; $length; $i++) {
			$nullArray[] = null;
		}
		return $nullArray;
	}
	
	function makeImpl($test, $maskPattern) {

		$this-&gt;moduleCount = $this-&gt;typeNumber * 4 + 17;

		$this-&gt;modules = array();
		for ($i = 0; $i &lt; $this-&gt;moduleCount; $i++) {
			$this-&gt;modules[] = QRCode::createNullArray($this-&gt;moduleCount);
		}

		$this-&gt;setupPositionProbePattern(0, 0);
		$this-&gt;setupPositionProbePattern($this-&gt;moduleCount - 7, 0);
		$this-&gt;setupPositionProbePattern(0, $this-&gt;moduleCount - 7);
		
		$this-&gt;setupPositionAdjustPattern();
		$this-&gt;setupTimingPattern();

		$this-&gt;setupTypeInfo($test, $maskPattern);

		if ($this-&gt;typeNumber &gt;= 7) {
			$this-&gt;setupTypeNumber($test);
		}
		
		$dataArray = $this-&gt;qrDataList;

		$data = QRCode::createData($this-&gt;typeNumber, $this-&gt;errorCorrectLevel, $dataArray);

		$this-&gt;mapData($data, $maskPattern);
	}
	
	function mapData(&amp;$data, $maskPattern) {
		
		$inc = -1;
		$row = $this-&gt;moduleCount - 1;
		$bitIndex = 7;
		$byteIndex = 0;
		
		for ($col = $this-&gt;moduleCount - 1; $col &gt; 0; $col -= 2) {

			if ($col == 6) $col--;

			while (true) {
				
				for ($c = 0; $c &lt; 2; $c++) {
					
					if ($this-&gt;modules[$row][$col - $c] === null) {
						
						$dark = false;

						if ($byteIndex &lt; count($data) ) {
							$dark = ( ( ($data[$byteIndex] &gt;&gt; $bitIndex) &amp; 1) == 1);
						}

						$mask = QRUtil::getMask($maskPattern, $row, $col - $c);

						if ($mask) {
							$dark = !$dark;
						}

						$this-&gt;modules[$row][$col - $c] = $dark;
						$bitIndex--;

						if ($bitIndex == -1) {
							$byteIndex++;
							$bitIndex = 7;
						}
					}
				}
								
				$row += $inc;

				if ($row &lt; 0 || $this-&gt;moduleCount &lt;= $row) {
					$row -= $inc;
					$inc = -$inc;
					break;
				}
			}
		}
	}
	
	function setupPositionAdjustPattern() {

		$pos = QRUtil::getPatternPosition($this-&gt;typeNumber);

		for ($i = 0; $i &lt; count($pos); $i++) {

			for ($j = 0; $j &lt; count($pos); $j++) {

				$row = $pos[$i];
				$col = $pos[$j];
				
				if ($this-&gt;modules[$row][$col] !== null) {
					continue;
				}
				
				for ($r = -2; $r &lt;= 2; $r++) {

					for ($c = -2; $c &lt;= 2; $c++) {

						if ($r == -2 || $r == 2 || $c == -2 || $c == 2 
								|| ($r == 0 &amp;&amp; $c == 0) ) {
							$this-&gt;modules[$row + $r][$col + $c] = true;
						} else {
							$this-&gt;modules[$row + $r][$col + $c] = false;
						}
					}
				}
			}
		}
	}

	function setupPositionProbePattern($row, $col) {

		for ($r = -1; $r &lt;= 7; $r++) {

			for ($c = -1; $c &lt;= 7; $c++) {

				if ($row + $r &lt;= -1 || $this-&gt;moduleCount &lt;= $row + $r 
						|| $col + $c &lt;= -1 || $this-&gt;moduleCount &lt;= $col + $c) {
					continue;
				}
					
				if ( (0 &lt;= $r &amp;&amp; $r &lt;= 6 &amp;&amp; ($c == 0 || $c == 6) )
						|| (0 &lt;= $c &amp;&amp; $c &lt;= 6 &amp;&amp; ($r == 0 || $r == 6) )
						|| (2 &lt;= $r &amp;&amp; $r &lt;= 4 &amp;&amp; 2 &lt;= $c &amp;&amp; $c &lt;= 4) ) {
					$this-&gt;modules[$row + $r][$col + $c] = true;
				} else {
					$this-&gt;modules[$row + $r][$col + $c] = false;
				}
			}		
		}		
	}

	function setupTimingPattern() {

		for ($r = 8; $r &lt; $this-&gt;moduleCount - 8; $r++) {
			if ($this-&gt;modules[$r][6] !== null) {
				continue;
			}
			$this-&gt;modules[$r][6] = ($r % 2 == 0);
		}

		for ($c = 8; $c &lt; $this-&gt;moduleCount - 8; $c++) {
			if ($this-&gt;modules[6][$c] !== null) {
				continue;
			}
			$this-&gt;modules[6][$c] = ($c % 2 == 0);
		}
	}

	function setupTypeNumber($test) {

		$bits = QRUtil::getBCHTypeNumber($this-&gt;typeNumber);

		for ($i = 0; $i &lt; 18; $i++) {
			$mod = (!$test &amp;&amp; ( ($bits &gt;&gt; $i) &amp; 1) == 1);
			$this-&gt;modules[floor($i / 3)][$i % 3 + $this-&gt;moduleCount - 8 - 3] = $mod;
		}

		for ($i = 0; $i &lt; 18; $i++) {
			$mod = (!$test &amp;&amp; ( ($bits &gt;&gt; $i) &amp; 1) == 1);
			$this-&gt;modules[$i % 3 + $this-&gt;moduleCount - 8 - 3][floor($i / 3)] = $mod;
		}
	}
	
	function setupTypeInfo($test, $maskPattern) {

		$data = ($this-&gt;errorCorrectLevel &lt;&lt; 3) | $maskPattern;
		$bits = QRUtil::getBCHTypeInfo($data);

		for ($i = 0; $i &lt; 15; $i++) {

			$mod = (!$test &amp;&amp; ( ($bits &gt;&gt; $i) &amp; 1) == 1);

			if ($i &lt; 6) {
				$this-&gt;modules[$i][8] = $mod;
			} else if ($i &lt; 8) {
				$this-&gt;modules[$i + 1][8] = $mod;
			} else {
				$this-&gt;modules[$this-&gt;moduleCount - 15 + $i][8] = $mod;
			}
		}

		for ($i = 0; $i &lt; 15; $i++) {

			$mod = (!$test &amp;&amp; ( ($bits &gt;&gt; $i) &amp; 1) == 1);
			
			if ($i &lt; 8) {
				$this-&gt;modules[8][$this-&gt;moduleCount - $i - 1] = $mod;
			} else if ($i &lt; 9) {
				$this-&gt;modules[8][15 - $i - 1 + 1] = $mod;
			} else {
				$this-&gt;modules[8][15 - $i - 1] = $mod;
			}
		}

		$this-&gt;modules[$this-&gt;moduleCount - 8][8] = !$test;
	}
	
	function createData($typeNumber, $errorCorrectLevel, $dataArray) {
		
		$rsBlocks = QRRSBlock::getRSBlocks($typeNumber, $errorCorrectLevel);
		
		$buffer = new QRBitBuffer();
		
		for ($i = 0; $i &lt; count($dataArray); $i++) {
			$data = $dataArray[$i];
			$buffer-&gt;put($data-&gt;getMode(), 4);
			$buffer-&gt;put($data-&gt;getLength(), $data-&gt;getLengthInBits($typeNumber) );
			$data-&gt;write($buffer); 
		}

		$totalDataCount = 0;
		for ($i = 0; $i &lt; count($rsBlocks); $i++) {
			$totalDataCount += $rsBlocks[$i]-&gt;getDataCount();
		}

		if ($buffer-&gt;getLengthInBits() &gt; $totalDataCount * 8) {
			trigger_error("code length overflow. ("
				. $buffer-&gt;getLengthInBits()
				. "&gt;"
				.  $totalDataCount * 8
				. ")", E_USER_ERROR);
		}

		// end code.
		if ($buffer-&gt;getLengthInBits() + 4 &lt;= $totalDataCount * 8) {
			$buffer-&gt;put(0, 4);
		}

		// padding
		while ($buffer-&gt;getLengthInBits() % 8 != 0) {
			$buffer-&gt;putBit(false);
		}

		// padding
		while (true) {
			
			if ($buffer-&gt;getLengthInBits() &gt;= $totalDataCount * 8) {
				break;
			}
			$buffer-&gt;put(QR_PAD0, 8);
			
			if ($buffer-&gt;getLengthInBits() &gt;= $totalDataCount * 8) {
				break;
			}
			$buffer-&gt;put(QR_PAD1, 8);
		}

		return QRCode::createBytes($buffer, $rsBlocks);
	}
	
	function createBytes(&amp;$buffer, &amp;$rsBlocks) {

		$offset = 0;
		
		$maxDcCount = 0;
		$maxEcCount = 0;
		
		$dcdata = QRCode::createNullArray(count($rsBlocks) );
		$ecdata = QRCode::createNullArray(count($rsBlocks) );
		
		for ($r = 0; $r &lt; count($rsBlocks); $r++) {

			$dcCount = $rsBlocks[$r]-&gt;getDataCount();
			$ecCount = $rsBlocks[$r]-&gt;getTotalCount() - $dcCount;

			$maxDcCount = max($maxDcCount, $dcCount);
			$maxEcCount = max($maxEcCount, $ecCount);
			
			$dcdata[$r] = QRCode::createNullArray($dcCount);
			for ($i = 0; $i &lt; count($dcdata[$r]); $i++) {
				$bdata = $buffer-&gt;getBuffer();
				$dcdata[$r][$i] = 0xff &amp; $bdata[$i + $offset];
			}
			$offset += $dcCount;
			
		    $rsPoly = QRUtil::getErrorCorrectPolynomial($ecCount);
			$rawPoly = new QRPolynomial($dcdata[$r], $rsPoly-&gt;getLength() - 1);

			$modPoly = $rawPoly-&gt;mod($rsPoly);
			$ecdata[$r] = QRCode::createNullArray($rsPoly-&gt;getLength() - 1);
			for ($i = 0; $i &lt; count($ecdata[$r]); $i++) {
				$modIndex = $i + $modPoly-&gt;getLength() - count($ecdata[$r]);
				$ecdata[$r][$i] = ($modIndex &gt;= 0)? $modPoly-&gt;get($modIndex) : 0;
			}
		}

		$totalCodeCount = 0;
		for ($i = 0; $i &lt; count($rsBlocks); $i++) {
			$totalCodeCount += $rsBlocks[$i]-&gt;getTotalCount();
		}

		$data = QRCode::createNullArray($totalCodeCount);

		$index = 0;

		for ($i = 0; $i &lt; $maxDcCount; $i++) {
			for ($r = 0; $r &lt; count($rsBlocks); $r++) {
				if ($i &lt; count($dcdata[$r]) ) {
					$data[$index++] = $dcdata[$r][$i];
				}
			}
		}

		for ($i = 0; $i &lt; $maxEcCount; $i++) {
			for ($r = 0; $r &lt; count($rsBlocks); $r++) {
				if ($i &lt; count($ecdata[$r]) ) {
					$data[$index++] = $ecdata[$r][$i];
				}
			}
		}

		return $data;
	}

    function getMinimumQRCode($data, $errorCorrectLevel) {
		
        $mode = QRUtil::getMode($data);

        $qr = new QRCode();
        $qr-&gt;setErrorCorrectLevel($errorCorrectLevel);
        $qr-&gt;addData($data, $mode);

		$qrData = $qr-&gt;getData(0);
        $length = $qrData-&gt;getLength();

		for ($typeNumber = 1; $typeNumber &lt;= 10; $typeNumber++) {
            if ($length &lt;= QRUtil::getMaxLength($typeNumber, $mode, $errorCorrectLevel) ) {
                $qr-&gt;setTypeNumber($typeNumber);
                break;
            }
		}

        $qr-&gt;make();

        return $qr;
    }

	function createImage($size = 2, $margin = 2) {
		
		$image_size = $this-&gt;getModuleCount() * $size + $margin * 2;
		
		$image = imagecreatetruecolor($image_size, $image_size);

		$black = imagecolorallocate($image, 0, 0, 0); 
		$white = imagecolorallocate($image, 255, 255, 255); 

		imagefilledrectangle($image, 0, 0, $image_size, $image_size, $white);

		for ($r = 0; $r &lt; $this-&gt;getModuleCount(); $r++) {
			for ($c = 0; $c &lt; $this-&gt;getModuleCount(); $c++) {
				if ($this-&gt;isDark($r, $c) ) {
				
					imagefilledrectangle($image,
					  	$margin + $c * $size,
					  	$margin + $r * $size, 
					  	$margin + ($c + 1) * $size - 1, 
					  	$margin + ($r + 1) * $size - 1,
					  	$black);
				}
			} 
		} 

		return $image;
	}

	
	function printHTML($size = "2px") {

		$style = "border-style:none;border-collapse:collapse;margin:0px;padding:0px;";

		print("&lt;table style='$style'&gt;");

		for ($r = 0; $r &lt; $this-&gt;getModuleCount(); $r++) {

			print("&lt;tr style='$style'&gt;");

			for ($c = 0; $c &lt; $this-&gt;getModuleCount(); $c++) {
				$color = $this-&gt;isDark($r, $c)? "#000000" : "#ffffff";
				print("&lt;td style='$style;width:$size;height:$size;background-color:$color'&gt;&lt;/td&gt;");
			}

			print("&lt;/tr&gt;");
		}
		
		print("&lt;/table&gt;");
	}
}

//---------------------------------------------------------------
// QRUtil
//---------------------------------------------------------------

$QR_PATTERN_POSITION_TABLE = array(
	array(),
	array(6, 18),
	array(6, 22),
	array(6, 26),
	array(6, 30),
	array(6, 34),
	array(6, 22, 38),
	array(6, 24, 42),
	array(6, 26, 46),
	array(6, 28, 50),
	array(6, 30, 54),		
	array(6, 32, 58),
	array(6, 34, 62),
	array(6, 26, 46, 66),
	array(6, 26, 48, 70),
	array(6, 26, 50, 74),
	array(6, 30, 54, 78),
	array(6, 30, 56, 82),
	array(6, 30, 58, 86),
	array(6, 34, 62, 90),
	array(6, 28, 50, 72, 94),
	array(6, 26, 50, 74, 98),
	array(6, 30, 54, 78, 102),
	array(6, 28, 54, 80, 106),
	array(6, 32, 58, 84, 110),
	array(6, 30, 58, 86, 114),
	array(6, 34, 62, 90, 118),
	array(6, 26, 50, 74, 98, 122),
	array(6, 30, 54, 78, 102, 126),
	array(6, 26, 52, 78, 104, 130),
	array(6, 30, 56, 82, 108, 134),
	array(6, 34, 60, 86, 112, 138),
	array(6, 30, 58, 86, 114, 142),
	array(6, 34, 62, 90, 118, 146),
	array(6, 30, 54, 78, 102, 126, 150),
	array(6, 24, 50, 76, 102, 128, 154),
	array(6, 28, 54, 80, 106, 132, 158),
	array(6, 32, 58, 84, 110, 136, 162),
	array(6, 26, 54, 82, 110, 138, 166),
	array(6, 30, 58, 86, 114, 142, 170)
);

$QR_MAX_LENGTH = array(
    array( array(41,  25,  17,  10),  array(34,  20,  14,  8),   array(27,  16,  11,  7),  array(17,  10,  7,   4) ),
    array( array(77,  47,  32,  20),  array(63,  38,  26,  16),  array(48,  29,  20,  12), array(34,  20,  14,  8) ),
    array( array(127, 77,  53,  32),  array(101, 61,  42,  26),  array(77,  47,  32,  20), array(58,  35,  24,  15) ),
    array( array(187, 114, 78,  48),  array(149, 90,  62,  38),  array(111, 67,  46,  28), array(82,  50,  34,  21) ),
    array( array(255, 154, 106, 65),  array(202, 122, 84,  52),  array(144, 87,  60,  37), array(106, 64,  44,  27) ),
    array( array(322, 195, 134, 82),  array(255, 154, 106, 65),  array(178, 108, 74,  45), array(139, 84,  58,  36) ),
    array( array(370, 224, 154, 95),  array(293, 178, 122, 75),  array(207, 125, 86,  53), array(154, 93,  64,  39) ),
    array( array(461, 279, 192, 118), array(365, 221, 152, 93),  array(259, 157, 108, 66), array(202, 122, 84,  52) ),
    array( array(552, 335, 230, 141), array(432, 262, 180, 111), array(312, 189, 130, 80), array(235, 143, 98,  60) ),
    array( array(652, 395, 271, 167), array(513, 311, 213, 131), array(364, 221, 151, 93), array(288, 174, 119, 74) )
);


define("QR_G15", (1 &lt;&lt; 10) | (1 &lt;&lt; 8) | (1 &lt;&lt; 5)
	| (1 &lt;&lt; 4) | (1 &lt;&lt; 2) | (1 &lt;&lt; 1) | (1 &lt;&lt; 0) );

define("QR_G18", (1 &lt;&lt; 12) | (1 &lt;&lt; 11) | (1 &lt;&lt; 10)
	| (1 &lt;&lt; 9) | (1 &lt;&lt; 8) | (1 &lt;&lt; 5) | (1 &lt;&lt; 2) | (1 &lt;&lt; 0) );

define("QR_G15_MASK", (1 &lt;&lt; 14) | (1 &lt;&lt; 12) | (1 &lt;&lt; 10)
	| (1 &lt;&lt; 4) | (1 &lt;&lt; 1) );

class QRUtil {

	function getPatternPosition($typeNumber) {
		global $QR_PATTERN_POSITION_TABLE;
		return $QR_PATTERN_POSITION_TABLE[$typeNumber - 1];
	}

    function getMaxLength($typeNumber, $mode, $errorCorrectLevel) {
		
		global $QR_MAX_LENGTH;
		
        $t = $typeNumber - 1;
        $e = 0;
        $m = 0;

        switch($errorCorrectLevel) {
        case QR_ERROR_CORRECT_LEVEL_L : $e = 0; break;
        case QR_ERROR_CORRECT_LEVEL_M : $e = 1; break;
        case QR_ERROR_CORRECT_LEVEL_Q : $e = 2; break;
        case QR_ERROR_CORRECT_LEVEL_H : $e = 3; break;
        default :
			trigger_error("e:$errorCorrectLevel", E_USER_ERROR);
        }

        switch($mode) {
        case QR_MODE_NUMBER    : $m = 0; break;
        case QR_MODE_ALPHA_NUM : $m = 1; break;
        case QR_MODE_8BIT_BYTE : $m = 2; break;
        case QR_MODE_KANJI     : $m = 3; break;
        default :
			trigger_error("m:$mode", E_USER_ERROR);
        }

        return $QR_MAX_LENGTH[$t][$e][$m];
    }

	function getErrorCorrectPolynomial($errorCorrectLength) {
			
		$a = new QRPolynomial(array(1) );

		for ($i = 0; $i &lt; $errorCorrectLength; $i++) {
			$a = $a-&gt;multiply(new QRPolynomial(array(1, QRMath::gexp($i) ) ) );
		}

		return $a;
	}
		
	function getMask($maskPattern, $i, $j) {
		
		switch ($maskPattern) {
			
		case QR_MASK_PATTERN000 : return ($i + $j) % 2 == 0;
		case QR_MASK_PATTERN001 : return $i % 2 == 0;
		case QR_MASK_PATTERN010 : return $j % 3 == 0;
		case QR_MASK_PATTERN011 : return ($i + $j) % 3 == 0;
		case QR_MASK_PATTERN100 : return (floor($i / 2) + floor($j / 3) ) % 2 == 0;
		case QR_MASK_PATTERN101 : return ($i * $j) % 2 + ($i * $j) % 3 == 0;
		case QR_MASK_PATTERN110 : return ( ($i * $j) % 2 + ($i * $j) % 3) % 2 == 0;
		case QR_MASK_PATTERN111 : return ( ($i * $j) % 3 + ($i + $j) % 2) % 2 == 0;

		default :
			trigger_error("mask:$maskPattern", E_USER_ERROR);
		}
	}	

	function getLostPoint($qrCode) {
		
		$moduleCount = $qrCode-&gt;getModuleCount();
		
		$lostPoint = 0;
		

		// LEVEL1
		
		for ($row = 0; $row &lt; $moduleCount; $row++) {

			for ($col = 0; $col &lt; $moduleCount; $col++) {

				$sameCount = 0;
				$dark = $qrCode-&gt;isDark($row, $col);
				
				for ($r = -1; $r &lt;= 1; $r++) {

					if ($row + $r &lt; 0 || $moduleCount &lt;= $row + $r) {
						continue;
					}

					for ($c = -1; $c &lt;= 1; $c++) {

						if ($col + $c &lt; 0 || $moduleCount &lt;= $col + $c) {
							continue;
						}

						if ($r == 0 &amp;&amp; $c == 0) {
							continue;
						}

						if ($dark == $qrCode-&gt;isDark($row + $r, $col + $c) ) {
							$sameCount++;
						}
					}
				}

				if ($sameCount &gt; 5) {
					$lostPoint += (3 + $sameCount - 5);
				}
			}
		}

		// LEVEL2

		for ($row = 0; $row &lt; $moduleCount - 1; $row++) {
			for ($col = 0; $col &lt; $moduleCount - 1; $col++) {
				$count = 0;
				if ($qrCode-&gt;isDark($row,     $col    ) ) $count++;
				if ($qrCode-&gt;isDark($row + 1, $col    ) ) $count++;
				if ($qrCode-&gt;isDark($row,     $col + 1) ) $count++;
				if ($qrCode-&gt;isDark($row + 1, $col + 1) ) $count++;
				if ($count == 0 || $count == 4) {
					$lostPoint += 3;
				}
			}
		}

		// LEVEL3

		for ($row = 0; $row &lt; $moduleCount; $row++) {
			for ($col = 0; $col &lt; $moduleCount - 6; $col++) {
				if ($qrCode-&gt;isDark($row, $col)
						&amp;&amp; !$qrCode-&gt;isDark($row, $col + 1)
						&amp;&amp;  $qrCode-&gt;isDark($row, $col + 2)
						&amp;&amp;  $qrCode-&gt;isDark($row, $col + 3)
						&amp;&amp;  $qrCode-&gt;isDark($row, $col + 4)
						&amp;&amp; !$qrCode-&gt;isDark($row, $col + 5)
						&amp;&amp;  $qrCode-&gt;isDark($row, $col + 6) ) {
					$lostPoint += 40;
				}
			}
		}

		for ($col = 0; $col &lt; $moduleCount; $col++) {
			for ($row = 0; $row &lt; $moduleCount - 6; $row++) {
				if ($qrCode-&gt;isDark($row, $col)
						&amp;&amp; !$qrCode-&gt;isDark($row + 1, $col)
						&amp;&amp;  $qrCode-&gt;isDark($row + 2, $col)
						&amp;&amp;  $qrCode-&gt;isDark($row + 3, $col)
						&amp;&amp;  $qrCode-&gt;isDark($row + 4, $col)
						&amp;&amp; !$qrCode-&gt;isDark($row + 5, $col)
						&amp;&amp;  $qrCode-&gt;isDark($row + 6, $col) ) {
					$lostPoint += 40;
				}
			}
		}

		// LEVEL4
		
		$darkCount = 0;

		for ($col = 0; $col &lt; $moduleCount; $col++) {
			for ($row = 0; $row &lt; $moduleCount; $row++) {
				if ($qrCode-&gt;isDark($row, $col) ) {
					$darkCount++;
				}
			}
		}
		
		$ratio = abs(100 * $darkCount / $moduleCount / $moduleCount - 50) / 5;
		$lostPoint += $ratio * 10;
		
		return $lostPoint;		
	}

	function getMode($s) {
		if (QRUtil::isAlphaNum($s) ) {
			if (QRUtil::isNumber($s) ) {
				return QR_MODE_NUMBER;
			}
			return QR_MODE_ALPHA_NUM;
		} else if (QRUtil::isKanji($s) ) {
			return QR_MODE_KANJI;
		} else {
			return QR_MODE_8BIT_BYTE;
		}
	}
		
	function isNumber($s) {
		for ($i = 0; $i &lt; strlen($s); $i++) {
			$c = ord($s[$i]);
			if (!(QRUtil::toCharCode('0') &lt;= $c &amp;&amp; $c &lt;= QRUtil::toCharCode('9') ) ) {
				return false;
			}
		}
		return true;
	}

	function isAlphaNum($s) {
		for ($i = 0; $i &lt; strlen($s); $i++) {
			$c = ord($s[$i]);
			if (!(QRUtil::toCharCode('0') &lt;= $c &amp;&amp; $c &lt;= QRUtil::toCharCode('9') ) 
				&amp;&amp; !(QRUtil::toCharCode('A') &lt;= $c &amp;&amp; $c &lt;= QRUtil::toCharCode('Z') ) 
					&amp;&amp; strpos(" $%*+-./:", $s[$i]) === false) {
				return false;
			}
		}
		return true;
	}

	function isKanji($s) {

		$data = $s;

		$i = 0;

		while ($i + 1 &lt; strlen($data) ) {
			
			$c = ( (0xff &amp; ord($data[$i]) ) &lt;&lt; 8) | (0xff &amp; ord($data[$i + 1]) );

			if (!(0x8140 &lt;= $c &amp;&amp; $c &lt;= 0x9FFC) &amp;&amp; !(0xE040 &lt;= $c &amp;&amp; $c &lt;= 0xEBBF) ) {
				return false;
			}
			
			$i += 2;
		}

		if ($i &lt; strlen($data) ) {
			return false;
		}
		
		return true;
	}
	
	function toCharCode($s) {
		return ord($s[0]);
	}

	function getBCHTypeInfo($data) {
		$d = $data &lt;&lt; 10;
		while (QRUtil::getBCHDigit($d) - QRUtil::getBCHDigit(QR_G15) &gt;= 0) {
			$d ^= (QR_G15 &lt;&lt; (QRUtil::getBCHDigit($d) - QRUtil::getBCHDigit(QR_G15) ) ); 	
		}
		return ( ($data &lt;&lt; 10) | $d) ^ QR_G15_MASK;
	}

	function getBCHTypeNumber($data) {
		$d = $data &lt;&lt; 12;
		while (QRUtil::getBCHDigit($d) - QRUtil::getBCHDigit(QR_G18) &gt;= 0) {
			$d ^= (QR_G18 &lt;&lt; (QRUtil::getBCHDigit($d) - QRUtil::getBCHDigit(QR_G18) ) ); 	
		}
		return ($data &lt;&lt; 12) | $d;
	}
	
	function getBCHDigit($data) {

		$digit = 0;

		while ($data != 0) {
			$digit++;
			$data &gt;&gt;= 1;
		}

		return $digit;
	}
}

//---------------------------------------------------------------
// QRRSBlock
//---------------------------------------------------------------

$QR_RS_BLOCK_TABLE = array(

	// L
	// M
	// Q
	// H

	// 1
	array(1, 26, 19),
	array(1, 26, 16),
	array(1, 26, 13),
	array(1, 26, 9),
	
	// 2
	array(1, 44, 34),
	array(1, 44, 28),
	array(1, 44, 22),
	array(1, 44, 16),

	// 3
	array(1, 70, 55),
	array(1, 70, 44),
	array(2, 35, 17),
	array(2, 35, 13),

	// 4		
	array(1, 100, 80),
	array(2, 50, 32),
	array(2, 50, 24),
	array(4, 25, 9),
	
	// 5
	array(1, 134, 108),
	array(2, 67, 43),
	array(2, 33, 15, 2, 34, 16),
	array(2, 33, 11, 2, 34, 12),
	
	// 6
	array(2, 86, 68),
	array(4, 43, 27),
	array(4, 43, 19),
	array(4, 43, 15),
	
	// 7		
	array(2, 98, 78),
	array(4, 49, 31),
	array(2, 32, 14, 4, 33, 15),
	array(4, 39, 13, 1, 40, 14),
	
	// 8
	array(2, 121, 97),
	array(2, 60, 38, 2, 61, 39),
	array(4, 40, 18, 2, 41, 19),
	array(4, 40, 14, 2, 41, 15),
	
	// 9
	array(2, 146, 116),
	array(3, 58, 36, 2, 59, 37),
	array(4, 36, 16, 4, 37, 17),
	array(4, 36, 12, 4, 37, 13),
	
	// 10		
	array(2, 86, 68, 2, 87, 69),
	array(4, 69, 43, 1, 70, 44),
	array(6, 43, 19, 2, 44, 20),
	array(6, 43, 15, 2, 44, 16)

);

class QRRSBlock {

	var $totalCount;
	var $dataCount;
	
	function QRRSBlock($totalCount, $dataCount) {
		$this-&gt;totalCount = $totalCount;
		$this-&gt;dataCount  = $dataCount;
	}
	
	function getDataCount() {
		return $this-&gt;dataCount;
	}
	
	function getTotalCount() {
		return $this-&gt;totalCount;
	}

	function getRSBlocks($typeNumber, $errorCorrectLevel) {
		
		$rsBlock = QRRSBlock::getRsBlockTable($typeNumber, $errorCorrectLevel);
		$length = count($rsBlock) / 3;

		$list = array();
						
		for ($i = 0; $i &lt; $length; $i++) {

			$count = $rsBlock[$i * 3 + 0];
			$totalCount = $rsBlock[$i * 3 + 1];
			$dataCount  = $rsBlock[$i * 3 + 2];

			for ($j = 0; $j &lt; $count; $j++) {
				$list[] = new QRRSBlock($totalCount, $dataCount);	
			}
		}
		
		return $list;
	}

	function getRsBlockTable($typeNumber, $errorCorrectLevel) {
		
		global $QR_RS_BLOCK_TABLE;
		
		switch($errorCorrectLevel) {
		case QR_ERROR_CORRECT_LEVEL_L :
			return $QR_RS_BLOCK_TABLE[($typeNumber - 1) * 4 + 0];
		case QR_ERROR_CORRECT_LEVEL_M :
			return $QR_RS_BLOCK_TABLE[($typeNumber - 1) * 4 + 1];
		case QR_ERROR_CORRECT_LEVEL_Q :
			return $QR_RS_BLOCK_TABLE[($typeNumber - 1) * 4 + 2];
		case QR_ERROR_CORRECT_LEVEL_H :
			return $QR_RS_BLOCK_TABLE[($typeNumber - 1) * 4 + 3];
		default :
			trigger_error("tn:$typeNumber/ecl:$errorCorrectLevel", E_USER_ERROR);
		}
	}
}

//---------------------------------------------------------------
// QRNumber
//---------------------------------------------------------------

class QRNumber extends QRData {

	function QRNumber($data) {
		QRData::QRData(QR_MODE_NUMBER, $data);
	}
	
	function write(&amp;$buffer) {

		$data = $this-&gt;getData();
		
		$i = 0;

		while ($i + 2 &lt; strlen($data) ) {
			$num = QRNumber::parseInt(substr($data, $i, 3) );
			$buffer-&gt;put($num, 10);
			$i += 3;
		}
		
		if ($i &lt; strlen($data) ) {
			
			if (strlen($data) - $i == 1) {
				$num = QRNumber::parseInt(substr($data, $i, $i + 1) );
				$buffer-&gt;put($num, 4);
			} else if (strlen($data) - $i == 2) {
				$num = QRNumber::parseInt(substr($data, $i, $i + 2) );
				$buffer-&gt;put($num, 7);
			}
		}
	}
	
	function getLength() {
		return strlen($this-&gt;getData() );
	}

	function parseInt($s) {

		$num = 0;
		for ($i = 0; $i &lt; strlen($s); $i++) {
			$num = $num * 10 + QRNumber::parseIntAt(ord($s[$i]) );
		}
		return $num;
	}

	function parseIntAt($c) {

		if (QRUtil::toCharCode('0') &lt;= $c &amp;&amp; $c &lt;= QRUtil::toCharCode('9') ) {
			return $c - QRUtil::toCharCode('0');
		}

		trigger_error("illegal char : $c", E_USER_ERROR);
	}
}

//---------------------------------------------------------------
// QRKanji
//---------------------------------------------------------------

class QRKanji extends QRData {

	function QRKanji($data) {
		QRData::QRData(QR_MODE_KANJI, $data);
	}
	
	function write(&amp;$buffer) {

		$data = $this-&gt;getData();

		$i = 0;

		while ($i + 1 &lt; strlen($data) ) {
			
			$c = ( (0xff &amp; ord($data[$i]) ) &lt;&lt; 8) | (0xff &amp; ord($data[$i + 1]) );

			if (0x8140 &lt;= $c &amp;&amp; $c &lt;= 0x9FFC) {
				$c -= 0x8140;
			} else if (0xE040 &lt;= $c &amp;&amp; $c &lt;= 0xEBBF) {
				$c -= 0xC140;
			} else {
				trigger_error("illegal char at " . ($i + 1) . "/$c", E_USER_ERROR);
			}
			
			$c = ( ($c &gt;&gt; 8) &amp; 0xff) * 0xC0 + ($c &amp; 0xff);

			$buffer-&gt;put($c, 13);
			
			$i += 2;
		}

		if ($i &lt; strlen($data) ) {
			trigger_error("illegal char at " . ($i + 1), E_USER_ERROR);
		}
	}
	
	function getLength() {
		return floor(strlen($this-&gt;getData() ) / 2);
	}	
}

//---------------------------------------------------------------
// QRAlphaNum
//---------------------------------------------------------------

class QRAlphaNum extends QRData {

	function QRAlphaNum($data) {
		QRData::QRData(QR_MODE_ALPHA_NUM, $data);
	}
	
	function write(&amp;$buffer) {

		$i = 0;
		$c = $this-&gt;getData();
		
		while ($i + 1 &lt; strlen($c) ) {
			$buffer-&gt;put(QRAlphaNum::getCode(ord($c[$i]) ) * 45
				+ QRAlphaNum::getCode(ord($c[$i + 1]) ), 11);
			$i += 2;
		}
		
		if ($i &lt; strlen($c) ) {
			$buffer-&gt;put(QRAlphaNum::getCode(ord($c[$i])), 6);
		}
	}
	
	function getLength() {
		return strlen($this-&gt;getData() );
	}
	
	function getCode($c) {

		if (QRUtil::toCharCode('0') &lt;= $c 
				&amp;&amp; $c &lt;= QRUtil::toCharCode('9') ) {
			return $c - QRUtil::toCharCode('0');
		} else if (QRUtil::toCharCode('A') &lt;= $c 
				&amp;&amp; $c &lt;= QRUtil::toCharCode('Z') ) {
			return $c - QRUtil::toCharCode('A') + 10;
		} else {
			switch ($c) {
			case QRUtil::toCharCode(' ') : return 36;
			case QRUtil::toCharCode('$') : return 37;
			case QRUtil::toCharCode('%') : return 38;
			case QRUtil::toCharCode('*') : return 39;
			case QRUtil::toCharCode('+') : return 40;
			case QRUtil::toCharCode('-') : return 41;
			case QRUtil::toCharCode('.') : return 42;
			case QRUtil::toCharCode('/') : return 43;
			case QRUtil::toCharCode(':') : return 44;
			default :
				trigger_error("illegal char : $c", E_USER_ERROR);
			}
		}

	}	
}

//---------------------------------------------------------------
// QR8BitByte
//---------------------------------------------------------------

class QR8BitByte extends QRData {
	
	function QR8BitByte($data) {
		QRData::QRData(QR_MODE_8BIT_BYTE, $data);
	}
	
	function write(&amp;$buffer) {

		$data = $this-&gt;getData();
		for ($i = 0; $i &lt; strlen($data); $i++) {
			$buffer-&gt;put(ord($data[$i]), 8);
		}
	}
	
	function getLength() {
		return strlen($this-&gt;getData() );
	}
}

//---------------------------------------------------------------
// QRData
//---------------------------------------------------------------

class QRData {
	
	var $mode;
	
	var $data;

	function QRData($mode, $data) {
		$this-&gt;mode = $mode;
		$this-&gt;data = $data;
	}
	
	function getMode() {
		return $this-&gt;mode;
	}
	
	function getData() {
		return $this-&gt;data;
	}
	
	function getLength() {
		trigger_error("not implemented.", E_USER_ERROR);
	}
	
	function write(&amp;$buffer) {
		trigger_error("not implemented.", E_USER_ERROR);
	}

	function getLengthInBits($type) {

		if (1 &lt;= $type &amp;&amp; $type &lt; 10) {

			// 1 - 9

			switch($this-&gt;mode) {
			case QR_MODE_NUMBER 	: return 10;
			case QR_MODE_ALPHA_NUM 	: return 9;
			case QR_MODE_8BIT_BYTE	: return 8;
			case QR_MODE_KANJI  	: return 8;
			default :
				trigger_error("mode:$this-&gt;mode", E_USER_ERROR);
			}

		} else if ($type &lt; 27) {

			// 10 - 26

			switch($this-&gt;mode) {
			case QR_MODE_NUMBER 	: return 12;
			case QR_MODE_ALPHA_NUM 	: return 11;
			case QR_MODE_8BIT_BYTE	: return 16;
			case QR_MODE_KANJI  	: return 10;
			default :
				trigger_error("mode:$this-&gt;mode", E_USER_ERROR);
			}

		} else if ($type &lt; 41) {

			// 27 - 40

			switch($this-&gt;mode) {
			case QR_MODE_NUMBER 	: return 14;
			case QR_MODE_ALPHA_NUM	: return 13;
			case QR_MODE_8BIT_BYTE	: return 16;
			case QR_MODE_KANJI  	: return 12;
			default :
				trigger_error("mode:$this-&gt;mode", E_USER_ERROR);
			}

		} else {
			trigger_error("mode:$this-&gt;mode", E_USER_ERROR);
		}
	}

}

//---------------------------------------------------------------
// QRMath
//---------------------------------------------------------------

$QR_MATH_EXP_TABLE = null;
$QR_MATH_LOG_TABLE = null;

class QRMath {

	function init() {		

		global $QR_MATH_EXP_TABLE;
		global $QR_MATH_LOG_TABLE;

		$QR_MATH_EXP_TABLE = QRMath::createNumArray(256);

		for ($i = 0; $i &lt; 8; $i++) {
			$QR_MATH_EXP_TABLE[$i] = 1 &lt;&lt; $i;
		}

		for ($i = 8; $i &lt; 256; $i++) {
			$QR_MATH_EXP_TABLE[$i] = $QR_MATH_EXP_TABLE[$i - 4]
				^ $QR_MATH_EXP_TABLE[$i - 5]
				^ $QR_MATH_EXP_TABLE[$i - 6]
				^ $QR_MATH_EXP_TABLE[$i - 8];
		}
		
		$QR_MATH_LOG_TABLE = QRMath::createNumArray(256);

		for ($i = 0; $i &lt; 255; $i++) {
			$QR_MATH_LOG_TABLE[$QR_MATH_EXP_TABLE[$i] ] = $i;
		}
	}

	function createNumArray($length) {
		$num_array = array();
		for ($i = 0; $i &lt; $length; $i++) {
			$num_array[] = 0;
		}
		return $num_array;
	}

	function glog($n) {

		global $QR_MATH_LOG_TABLE;

		if ($n &lt; 1) {
			trigger_error("log($n)", E_USER_ERROR);
		}
		
		return $QR_MATH_LOG_TABLE[$n];
	}
	
	function gexp($n) {

		global $QR_MATH_EXP_TABLE;

		while ($n &lt; 0) {
			$n += 255;
		}

		while ($n &gt;= 256) {
			$n -= 255;
		}

		return $QR_MATH_EXP_TABLE[$n];
	}
}

// init static table
QRMath::init();

//---------------------------------------------------------------
// QRPolynomial
//---------------------------------------------------------------

class QRPolynomial {

	var $num;

	function QRPolynomial($num, $shift = 0) {

		$offset = 0;

		while ($offset &lt; count($num) &amp;&amp; $num[$offset] == 0) {
			$offset++;
		}

		$this-&gt;num = QRMath::createNumArray(count($num) - $offset + $shift);
		for ($i = 0; $i &lt; count($num) - $offset; $i++) {
			$this-&gt;num[$i] = $num[$i + $offset];
		}
	}

	function get($index) {
		return $this-&gt;num[$index];
	}

	function getLength() {
		return count($this-&gt;num);
	}

	// PHP5
	function __toString() {
		return $this-&gt;toString();
	}
	
	function toString() {

		$buffer = "";

		for ($i = 0; $i &lt; $this-&gt;getLength(); $i++) {
			if ($i &gt; 0) {
				$buffer .= ",";
			}
			$buffer .= $this-&gt;get($i);
		}

		return $buffer;
	}
	
	function toLogString() {

		$buffer = "";

		for ($i = 0; $i &lt; $this-&gt;getLength(); $i++) {
			if ($i &gt; 0) {
				$buffer .= ",";
			}
			$buffer .= QRMath::glog($this-&gt;get(i) );
		}

		return $buffer;
	}
	
	function multiply($e) {

		$num = QRMath::createNumArray($this-&gt;getLength() + $e-&gt;getLength() - 1);

		for ($i = 0; $i &lt; $this-&gt;getLength(); $i++) {
			for ($j = 0; $j &lt; $e-&gt;getLength(); $j++) {
				$num[$i + $j] ^= QRMath::gexp(QRMath::glog($this-&gt;get($i) ) + QRMath::glog($e-&gt;get($j) ) );
			}
		}

		return new QRPolynomial($num);
	}
	
	function mod($e) {

		if ($this-&gt;getLength() - $e-&gt;getLength() &lt; 0) {
			return $this;
		}

		$ratio = QRMath::glog($this-&gt;get(0) ) - QRMath::glog($e-&gt;get(0) );

		$num = QRMath::createNumArray($this-&gt;getLength() );
		for ($i = 0; $i &lt; $this-&gt;getLength(); $i++) {
			$num[$i] = $this-&gt;get($i);
		}
		
		for ($i = 0; $i &lt; $e-&gt;getLength(); $i++) {
			$num[$i] ^= QRMath::gexp(QRMath::glog($e-&gt;get($i) ) + $ratio);
		}

		$newPolynomial = new QRPolynomial($num);
		return $newPolynomial-&gt;mod($e);
	}
}

//---------------------------------------------------------------
// Mode
//---------------------------------------------------------------

define("QR_MODE_NUMBER", 1 &lt;&lt; 0);
define("QR_MODE_ALPHA_NUM", 1 &lt;&lt; 1);
define("QR_MODE_8BIT_BYTE", 1 &lt;&lt; 2);
define("QR_MODE_KANJI", 1 &lt;&lt; 3);

//---------------------------------------------------------------
// MaskPattern
//---------------------------------------------------------------

define("QR_MASK_PATTERN000", 0);
define("QR_MASK_PATTERN001", 1);
define("QR_MASK_PATTERN010", 2);
define("QR_MASK_PATTERN011", 3);
define("QR_MASK_PATTERN100", 4);
define("QR_MASK_PATTERN101", 5);
define("QR_MASK_PATTERN110", 6);
define("QR_MASK_PATTERN111", 7);

//---------------------------------------------------------------
// ErrorCorrectLevel

// 7%.
define("QR_ERROR_CORRECT_LEVEL_L", 1);
// 15%.
define("QR_ERROR_CORRECT_LEVEL_M", 0);
// 25%.
define("QR_ERROR_CORRECT_LEVEL_Q", 3);
// 30%.
define("QR_ERROR_CORRECT_LEVEL_H", 2);


//---------------------------------------------------------------
// QRBitBuffer
//---------------------------------------------------------------

class QRBitBuffer {
	
	var $buffer;
	var $length;
	
	function QRBitBuffer() {
		$this-&gt;buffer = array();
		$this-&gt;length = 0;
	}

	function getBuffer() {
		return $this-&gt;buffer;
	}
		
	function getLengthInBits() {
		return $this-&gt;length;
	}

	function __toString() {
		$buffer = "";
		for ($i = 0; $i &lt; $this-&gt;getLengthInBits(); $i++) {
			$buffer .= $this-&gt;get($i)? '1' : '0';
		}
		return $buffer;
	}

	function get($index) {
		$bufIndex = floor($index / 8);
		return ( ($this-&gt;buffer[$bufIndex] &gt;&gt; (7 - $index % 8) ) &amp; 1) == 1;
	}

	function put($num, $length) {

		for ($i = 0; $i &lt; $length; $i++) {
			$this-&gt;putBit( ( ($num &gt;&gt; ($length - $i - 1) ) &amp; 1) == 1);
		}
	}
	
	function putBit($bit) {
		
		$bufIndex = floor($this-&gt;length / 8);
		if (count($this-&gt;buffer) &lt;= $bufIndex) {
			$this-&gt;buffer[] = 0;
		}
		
		if ($bit) {
			$this-&gt;buffer[$bufIndex] |= (0x80 &gt;&gt; ($this-&gt;length % 8) );
		}

		$this-&gt;length++;
	}
}

?&gt;</pre></body></html>