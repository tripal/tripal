<?php
/**
 * @file
 * Breaks an existing SQL file into chunks for import via php.
 *
 * What does this script attempt to achomplish?
 *   - Break a large SQL file into smaller chunks
 *   - Support multi-line SQL statements to ensure none are split
 *   - expected to be run on the command-line
 *   - does NOT require drush/drupal.
 *
 * Example usage:
 *   cd parts-v1.3
 *   php ../chunk_sql_file.php ../default_schema-1.3.sql default_schema-1.3.part
 *
 * The script will generate multiple lines of output showing you where
 * it split. Check this output to make sure that it didn't split within
 * the middle of a valid SQL statement.
 */

// -- PARAMETERS --------------------------------------------------------------
// (filepath) The full path of the file to be split into chunks.
$full_sql_filename = $argv[1];

// (string) The stub to use when creating files to contain the chunks.
$chunk_filestub = $argv[2];

// (number) The approximate size of the chunks in number of lines.
// Each file chunk will contain more lines then this number so be conservative.
$approx_chunk_size = 1000;

// -- SCRIPT PROPER -----------------------------------------------------------
// open the full SQL file which needs to be chunked.
$ORIG = fopen($full_sql_filename, 'r');
if (!$ORIG) {
	die('Unable to open file: '.$full_sql_filename);
}

// First set the starting variables.
$orig_linenum = 0;
$chunk_file_linenum = 0;
$chunk_file_i = 1;
$CHUNK = fopen($chunk_filestub . $chunk_file_i . '.sql', 'w');
$last_search_path = "SET search_path = chado,public;\n";
fwrite($CHUNK, $last_search_path);

// Now for each line...
while (($line = fgets($ORIG)) !== false) {

	// Make sure to change set search_path lines...
	if (preg_match('/search_path/',$line)) {
		$line = str_replace('public','chado',$line);
		$last_search_path = $line;
	}

	// Add the current line as no lines should be lost!
	fwrite($CHUNK, $line);
	$chunk_file_linenum++;
	$orig_linenum++;

	// If we are over the approximate chunk size...
	// we want to keep going until we reach a comment.
	if (($chunk_file_linenum > $approx_chunk_size) AND (preg_match('/^-+ [=\*][=\*]/', $line))) {
		print "Splitting the file at (line $orig_linenum) $line";
		fclose($CHUNK);
		$chunk_file_i++;
		$chunk_file_linenum = 0;
		$CHUNK = fopen($chunk_filestub . $chunk_file_i . '.sql', 'w');
		fwrite($CHUNK, $last_search_path);
	}
}
