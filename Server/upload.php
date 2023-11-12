<?php
$baseDir = $_SERVER['DOCUMENT_ROOT'] . "/";
$firmwareDir = "firmware/";
$latestVersionFile = $baseDir . $firmwareDir . "version.txt";
$constantFileName = "app.bin";

// Generate CRC32 table
function generate_crc32_table() {
    $polynomial = 0xEDB88320;
    $crc32_table = array();
    for ($i = 0; $i < 256; $i++) {
        $crc = $i;
        for ($j = 0; $j < 8; $j++) {
            $crc = ($crc & 1) ? (($crc >> 1) ^ $polynomial) : ($crc >> 1);
        }
        $crc32_table[$i] = $crc;
    }
    return $crc32_table;
}

// Calculate CRC32 for given data
function calculate_crc32($data) {
    $crc = 0xFFFFFFFF;
    $crc32_table = generate_crc32_table();
    $length = strlen($data);
    for ($i = 0; $i < $length; $i++) {
        $crc = ($crc >> 8) ^ $crc32_table[($crc ^ ord($data[$i])) & 0xFF];
    }
    return $crc ^ 0xFFFFFFFF;
}
if ($_FILES["fileToUpload"]["error"] == UPLOAD_ERR_OK) {
    // Extract the version from the file name
    $fileName = $_FILES["fileToUpload"]["name"];
    $version = "";
    if (preg_match('/^(.+)\.(\d+\.\d+\.\d+)\.bin$/', $fileName, $matches)) {
        $version = $matches[2];
        $baseFileName = $matches[1];
    } else {
        echo "Invalid file name format. File names must be in the format 'filename.version.bin'.";
        exit;
    }

    // Read the current highest version from version.txt
    $currentHighestVersion = '0.0.0'; // Default lowest version
    if (file_exists($latestVersionFile)) {
        $currentHighestVersion = file_get_contents($latestVersionFile);
    }

    // Compare the current version with the highest version
if (version_compare($version, $currentHighestVersion, '>') || $currentHighestVersion === '0.0.0') {
    // Update the highest version
    file_put_contents($latestVersionFile, $version);
}

    // Construct the version directory
    $versionDirectory = $baseDir . $firmwareDir . $version . "/";

    // Check if the directory already exists
    if (!file_exists($versionDirectory)) {
        // Create the version directory
        mkdir($versionDirectory, 0755, true);
    }

    // Calculate CRC32 for the new file
    $crc = calculate_crc32(file_get_contents($_FILES["fileToUpload"]["tmp_name"]));

    // Move the uploaded file to the version directory
    $targetFilePath = $versionDirectory . $constantFileName;
    if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $targetFilePath)) {
        // Update the CRC file for the new uploaded file
        $crc_filename = $versionDirectory . "crc.txt";
        file_put_contents($crc_filename, $crc);

        // Update the size file for the new uploaded file
        $size_filename = $versionDirectory . "size.txt";
        $fileSize = filesize($targetFilePath);
        file_put_contents($size_filename, $fileSize);

        echo "Firmware uploaded successfully. New version: $version";
    } else {
        echo "Sorry, there was an error uploading your file.";
    }
}
?>
