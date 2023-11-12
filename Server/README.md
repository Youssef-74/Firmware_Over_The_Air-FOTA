# Firmware Over The Air (FOTA) Update - Server Side
## Introduction
This PHP script is designed for server-side implementation, specifically tailored for Firmware Over The Air (FOTA) updates. It's intended to be hosted on a free web hosting area, providing a straightforward solution for updating firmware in a microcontroller-based project.

## Description
The script handles the upload of firmware files, manages version tracking, calculates CRC32 checksums, and organizes file storage in version-specific directories. It's designed to work within the constraints of a free web hosting environment.

## Features
1. File Upload:

* Accepts firmware files in the format 'filename.version.bin' (e.g., 'app.1.0.0.bin').
* Validates the file format before processing.
2. Version Tracking:

* Maintains a version.txt file to track the latest uploaded firmware version.
* Updates the version only if the uploaded version is greater than the current highest version.
3. CRC32 Calculation:

* Generates CRC32 checksums for uploaded firmware files.
* Uses a custom CRC32 calculation function.<br>
4. File Storage:

* Organizes uploaded firmware files in directories based on their versions.
* Creates version-specific directories for each firmware version.
## How to Use
1. Upload Firmware:

* Use the provided HTML form to select and upload firmware files.
* Ensure files are named in the format 'filename.version.bin'.<br> 
2. Version Tracking:

* The latest uploaded version is recorded in version.txt.
* Only updates if the uploaded version is greater than the current highest version.<br>
3. CRC32 Calculation:

* CRC32 checksums are calculated for each uploaded firmware file.<br>
4. File Storage:

* Files are organized into version-specific directories.
* Each version directory contains the firmware file (app.bin), CRC32 checksum file (crc.txt), and file size file (size.txt).
## Configuration
* Base Directory: Adjust the $baseDir variable to set the server's root directory.

* Firmware Directory: Update the $firmwareDir variable to specify the directory for storing firmware files.

* Custom CRC32 Function: Modify the calculate_crc32 function for a custom CRC32 calculation if needed.

## Requirements
* PHP with file upload and hash extension support.
* Web server environment for PHP execution.
## Important Notes
* Ensure proper permissions for file and directory creation.
* Validate the correctness of versioned filenames before upload.
