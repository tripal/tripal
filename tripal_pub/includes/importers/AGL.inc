<?php
/**
 *  
 */

/**
 *
 */
function tripal_pub_remote_search_AGL($search_array, $num_to_retrieve, $pager_id) {
  // get some values from the serach array  
  $num_criteria = $search_array['num_criteria'];
  $days = $search_array['days'];   

  // set some defaults
  $search_array['limit'] = $num_to_retrieve;
  
  // Build the query by iterating through the search array values
  $ccl = '';
  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = trim($search_array['criteria'][$i]['search_terms']);
    $scope = $search_array['criteria'][$i]['scope'];
    $is_phrase = $search_array['criteria'][$i]['is_phrase'];
    $op = $search_array['criteria'][$i]['operation'];
    if ($op) {
      $ccl .= " " . strtolower($op) . " ";
    }
    $search_terms = trim($search_terms);
    // if this is not a phrase then make sure the AND and OR are lower-case
    if (!$is_phrase) {
      $search_terms = preg_replace('/ OR /', ' or ', $search_terms);
      $search_terms = preg_replace('/ AND /', ' and ', $search_terms);
    }
    // else make sure the search terms are surrounded by quotes 
    else {
      $search_terms = "\"$search_terms\""; 
    }        
    $ccl .= ' (';
    if ($scope == 'title') {      
      $ccl .= "title=($search_terms)";
    }    
    elseif ($scope == 'author') {
      $ccl .= "author=($search_terms)";
    }    
    elseif ($scope == 'abstit') {
      $ccl .= "(title=($search_terms) or abstract=($search_terms))";
    }
    elseif ($scope == 'id') {
      $search_terms = preg_replace('/AGL:([^\s]*)/', 'id=($1)', $search_terms);
      $ccl .= $search_terms;
    }  
    else {
     $ccl .= "$search_terms"; 
    }
    $ccl .= ') ';
  } 

  if ($days) {
    // get the date of the day suggested
    $past_timestamp = time() - ($days * 86400);
    $past_date = getdate($past_timestamp); 
    $ccl .= " and (date>=" . sprintf("%04d%02d%02d.0", $past_date['year'], $past_date['mon'], $past_date['mday']) . ")";
  }

  // yaz_connect() prepares for a connection to a Z39.50 server. This function is non-blocking 
  // and does not attempt to establish a connection - it merely prepares a connect to be 
  // performed later when yaz_wait() is called.
  //$yazc = yaz_connect('agricola.nal.usda.gov:7090/voyager'); // NAL Catalog
  $yazc = yaz_connect('agricola.nal.usda.gov:7190/voyager');  // NAL Article Citation Database
    
  // use the USMARC record type.  But OPAC is also supported by Agricola
  yaz_syntax($yazc, "usmarc");
  
  // the search query is built using CCL, we need to first
  // configure it so it can map the attributes to defined identifiers
  // The attribute set used by AGL can be found at the bottom of this page:
  // http://agricola.nal.usda.gov/help/z3950.html  
  // 
  // More in depth details:  http://www.loc.gov/z3950/agency/bib1.html
  //
  // CCL Syntax: http://www.indexdata.com/yaz/doc/tools.html#CCL
  //
  $fields = array(
    "title"    => "u=4",
    "author"   => "u=1003",
    "abstract" => "u=62",
    "id"       => "u=12",
    "date"     => "u=1011 r=o p=3 s=100 ", 
  );
  yaz_ccl_conf($yazc, $fields);
  
  //dpm($ccl);
  if (!yaz_ccl_parse($yazc, $ccl, &$cclresult)) {
    drupal_set_message('Error parsing search string: ' . $cclresult["errorstring"], "error");
    watchdog('tripal_pub', 'Error: %errstr', array('%errstr' => $cclresult["errorstring"]), WATCHDOG_ERROR);
    return array();
  } 
  $search_str = $cclresult["rpn"];    
  
  $search_array['search_string'] = $search_str;
  //dpm($search_array);
    
  // save the YAZ connection in the session for use by other functions
  $_SESSION['tripal_pub_AGL_query'][$search_str]['yaz_connection'] = $yazc;
  
  // we want to get the list of pubs using the search terms but using a Drupal style pager
  $pubs = tripal_pager_callback('tripal_pub_AGL_range',  $num_to_retrieve, $pager_id, 
    'tripal_pub_AGL_count', $search_array);
  
  // close the connection
  unset($_SESSION['tripal_pub_AGL_query'][$search_str]['yaz_connection']);
  yaz_close($yazc);  
  
  return $pubs;
}
/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns a count of
 * the dataset to be paged.
 */
function tripal_pub_AGL_count($search_array) {
  $search_str = $search_array['search_string'];
  $days       = $search_array['days'];
  $limit      = $search_array['limit'];
  
  $yazc = $_SESSION['tripal_pub_AGL_query'][$search_str]['yaz_connection'];
  //yaz_sort($yazc, "1=31 id"); // sort by publication date descending
  if (!yaz_search($yazc, "rpn", $search_str)){
    $error_no = yaz_errno($yazc);
    $error_msg = yaz_error($yazc);
    $additional = yaz_addinfo($yazc);
    if ($additional != $error_msg) {
      $error_msg .= " $additional";
    }
    drupal_set_message("ERROR preparing search at AGL: ($error_no) $error_msg", "error");
    return 0;    
  }
  if (!yaz_wait()) {
    $error_no = yaz_errno($yazc);
    $error_msg = yaz_error($yazc);
    $additional = yaz_addinfo($yazc);
    if ($additional != $error_msg) {
      $error_msg .= " $additional";
    }
    drupal_set_message("ERROR waiting on search at AGL: ($error_no) $error_msg", "error");
    return 0;
  }
  
  // get the total number of results from the serach
  $count = yaz_hits($yazc);  
  $_SESSION['tripal_pub_AGL_query'][$search_str]['Count'] = $count; 
  return $count;
}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns the results
 * within the specified range
 */
function tripal_pub_AGL_range($search_array, $start = 0, $limit = 10) {
  $pubs = array();
  
  $search_str = $search_array['search_string'];
  $days       = $search_array['days'];
  $limit      = $search_array['limit'];
  
  $yazc  = $_SESSION['tripal_pub_AGL_query'][$search_str]['yaz_connection'];
  $count = $_SESSION['tripal_pub_AGL_query'][$search_str]['Count'];
  yaz_range($yazc, 1, $num_pubs); 
  if (!yaz_present($yazc)) {
    $error_no = yaz_errno($yazc);
    $error_msg = yaz_error($yazc);
    $additional = yaz_addinfo($yazc);
    if ($additional != $error_msg) {
      $error_msg .= " $additional";
    }
    drupal_set_message("ERROR waiting on search at AGL: ($error_no) $error_msg", "error");
    return $pubs;
  }  
     
  if ($start + $limit > $count) {
    $limit = $count - $start;
  }
  for($i = $start; $i < $start + $limit; $i++) { 
    $pub_xml = yaz_record($yazc, $i + 1, 'xml; charset=marc-8,utf-8');
    $pub     = tripal_pub_AGL_parse_pubxml($pub_xml);
    $pubs[]  = $pub;     
  } 
  return $pubs;
}

/*
 * Description of XML format:
 * http://www.loc.gov/marc/bibliographic/bdsummary.html
 * 
 */
function tripal_pub_AGL_parse_pubxml($pub_xml) {
  $pub = array();
  
  if (!$pub_xml) {
    return $pub;
  }
  
  // read the XML and iterate through it.
  $xml = new XMLReader();
  $xml->xml($pub_xml);
  while ($xml->read()) {    
    $element = $xml->name; 

    if ($xml->nodeType == XMLReader::ELEMENT and $element == 'controlfield') {      
      $tag = $xml->getAttribute('tag');
      $xml->read(); 
      $value = $xml->value;
      switch ($tag) {
        case '001':  // control number
          $pub['Publication Accession'] = $value;
          break;
        case '003':  // control number identifier
          break;
        case '005':  // datea nd time of latest transaction
          break;
        case '006':  // fixed-length data elemetns
          break;
        case '007':  // physical description fixed field
          break;
        case '008':  // fixed length data elements
          $month = array(
            '01' => 'Jan', '02' => 'Feb', '03' => 'Mar', 
            '04' => 'Apr', '05' => 'May', '06' => 'Jun', 
            '07' => 'Jul', '08' => 'Aug', '09' => 'Sep', 
            '10' => 'Oct', '11' => 'Nov', '12' => 'Dec'
          );
          $date0 = substr($value, 0, 6);  // date entered on file
          $date1 = substr($value, 7, 4);  // year of publication
          $date2 = substr($value, 11, 4); // month of publication
          $place = substr($value, 15, 3);
          $lang  = substr($value, 35, 3);
          if (preg_match('/\d\d\d\d/', $date1)) {
            $pub['Year'] = $date1;
            $pub['Publication Date'] = $date1;
          }
          if (preg_match('/\d\d/', $date2)) {
            $pub['Publication Date'] = $date1 . " " . $month[substr($date2, 0, 2)] . " " . substr($date2, 3, 2);
          }
          if (!preg_match('/\s+/', $place)) {
            $pub['Published Location'] = $place;
          }
          if (!preg_match('/\s+/', $lang)) {
            $pub['Language Abbr'] = $lang;
          }
          break;
        default:  // unhandled tag
          break;
      }          
    }
    elseif ($xml->nodeType == XMLReader::ELEMENT and $element == 'datafield') { 
      $tag  = $xml->getAttribute('tag');
      $ind1 = $xml->getAttribute('ind1');
      $ind2 = $xml->getAttribute('ind2');
      switch ($tag) {
        case '16':  // National Bibliographic Agency Control Number
          break;
        case '35':  // System Control Number
          $author = array();
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {                
              case 'a': // System control number
                $pub['Publication Accession'] = $value;
                break;
            }
          }
        case '40':  // Cataloging Source (NR)
          $author = array();
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {                
              case 'a':  // original cataolging agency
                $pub['Publication Database'] = $value;
                break;
            }
          }
          break;
        case '72':  // Subject Category Code
          break;
        case '100':  // main entry-personal name
          $author = tripal_pub_remote_search_AGL_get_author($xml, $ind1);
          $pub['Author List'][] = $author; 
          break;
        case '110':  // main entry-corporate nmae
          $author = array();
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {                
              case 'a': // Corporate name or jurisdiction name as entry elemen
                $author['Collective'] = $value;
                break;
              case 'b': // Subordinate unit
                $author['Collective'] .= ' ' . $value;
                break;
            }
          }
          $pub['Author List'][] = $author; 
          break;
        case '111':  // main entry-meeting name
          break;
        case '130':  // main entry-uniform title
          break;

        case '210':  // abbreviated title
          break;
        case '222':  // key title
          break;
        case '240':  // uniform title
          break;
        case '242':  // translation of title by cataloging agency
          break;
        case '243':  // collective uniform title
          break;
        case '245':  // title statement
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {          
            switch ($code) {                
              case 'a':
                $pub['Title'] = preg_replace('/\.$/', '', $value);
                break;
              case 'b':
                $pub['Title'] .= ' ' . $value;
                break;
              case 'h':
                $pub['Publication Model'] = $value;
                break;
            }
          }
          break;
        case '246':  // varying form of title
          break;
        case '247':  // former title
          break;            

        case '250':  // edition statement
          break;
        case '254':  // musicla presentation statement
          break;
        case '255':  // cartographic mathematical data
          break;
        case '256':  // computer file characteristics 
          break;
        case '257':  // country of producing entity
          break;
        case '258':  // philatelic issue data
          break;
        case '260':  // publication, distribution ,etc (imprint)
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {          
            switch ($code) {                
              case 'a':
                $pub['Published Location'] = $value;
                break;
              case 'b':
                $pub['Publisher'] = $value;
                break;                
              case 'c':
                $pub['Publication Date'] = $value;
                break;
            }
          }
          break;
        case '263':  // projected publication date
          break;
        case '264':  // production, publication, distribution, manufacture and copyright notice
          break;
        case '270':  // Address
          break;
          
        case '300':  // Address
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {          
            switch ($code) {                
              case 'a':
                $pages = $value;
                $pages = preg_replace('/^p\. /', '', $pages);
                $pages = preg_replace('/\.$/', '' , $pages);
                if(preg_match('/p$/', $pages)) {
                  // skip this, it's the number of pages not the page numbers
                } 
                else {
                  $pub['Pages'] = $pages;
                }
                break;
            }
          }
          break;                      


        case '500':  // series statements
          $pub['Notes'] = $value;
          break;
        case '504':  // Bibliography, Etc. Note
          break;
        case '520':  // Summary, etc
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {                
              case 'a':
                $pub['Abstract'] = $value;
                break;
            }
          }
          break;
        case '650':  // Subject Added Entry-Topical Term
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {
              case 'a':
                $pub['Keywords'][] = $value;
                break;  
            }
          }          
          break;
        case '653':  // Index Term-Uncontrolled
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {
              case 'a':
                $pub['Keywords'][] = $value;
                break;  
            }
          }
          break;
        case '700':  // Added Entry-Personal Name
          $author = tripal_pub_remote_search_AGL_get_author($xml, $ind1);
          $pub['Author List'][] = $author; 
          break;
        case '710':  // Added Entry-Corporate Name
          $author = array();
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {                
              case 'a': // Corporate name or jurisdiction name as entry elemen
                $author['Collective'] = $value;
                break;
              case 'b': // Subordinate unit
                $author['Collective'] .= ' ' . $value;
                break;
            }
          }
          $pub['Author List'][] = $author; 
          break;
        case '773': // host item entry
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {                
              case 't':                
                $pub['Journal Name'] = preg_replace('/\.$/', '', $value);
                break;
              case 'g':
                $matches = array();
                if (preg_match('/^(\d\d\d\d)/', $value, $matches)) {
                  $pub['Year'] = $matches[1];
                  $pub['Publication Date'] = $matches[1];
                }
                elseif (preg_match('/(.*?)(\.|\s+)\s*(\d+),\s(\d\d\d\d)/', $value, $matches)) {
                  $year = $matches[4];
                  $month = $matches[1];
                  $day = $matches[3];
                  $pub['Year'] = $year;
                  $pub['Publication Date'] = "$year $month $day"; 
                }
                elseif (preg_match('/\((.*?)(\.|\s+)(\d\d\d\d)\)/', $value, $matches)) {
                  $year = $matches[3];
                  $month = $matches[1];
                  $pub['Year'] = $year;
                  $pub['Publication Date'] = "$year $month"; 
                }
                elseif (preg_match('/^(.*?) (\d\d\d\d)/', $value, $matches)) {
                  $year = $matches[2];
                  $month = $matches[1];
                  $pub['Year'] = $year;
                  $pub['Publication Date'] = "$year $month"; 
                }
                if (preg_match('/v\. (.*?)(,|\s+)/', $value, $matches)) {
                  $pub['Volume'] = $matches[1];
                }
                if (preg_match('/v\. (.*?)(,|\s+)\((.*?)\)/', $value, $matches)) {
                  $pub['Volume'] = $matches[1];
                  $pub['Issue'] = $matches[3];
                }
                if (preg_match('/no\. (.*?)(\s|$)/', $value, $matches)) {
                  $pub['Issue'] = $matches[1];
                }
                break;
              case 'p':
                $pub['Journal Abbreviation'] = $value;
                break;
              case 'z':
                $pub['ISBN'] = $value;
                break;
            }
          }
          break;
        case '852': // Location (Where is the publication held)
          break;
        case '856': // Electronic Location and Access
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          foreach ($codes as $code => $value) {    
            switch ($code) {
              case 'u':
                $pub['URL'] = $value; 
                break;    
            }
          }
          break;
        default:
          $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
          $unhandled[$tag][] = $codes;
          break;
      }
    }
  }
  //dpm($unhandled);
  
  // build the Dbxref
  if ($pub['Publication Database'] != 'AGL') {
    
  }
  if ($pub['Publication Accession'] and $pub['Publication Database']) {
    $pub['Publication Dbxref'] = $pub['Publication Database'] . ":" . $pub['Publication Accession'];
    unset($pub['Publication Accession']);
    unset($pub['Publication Database']);
  }

  // build the full authors list
  foreach ($pub['Author List'] as $author) {
    if ($author['valid'] == 'N') {
      // skip non-valid entries.  A non-valid entry should have 
      // a corresponding corrected entry so we can saftely skip it.
      continue;
    }
    if ($author['Collective']) {
      $authors .= $author['Collective'] . ', ';
    }
    else {
      $authors .= $author['Surname'] . ' ' . $author['First Initials'] . ', '; 
    }             
  }
  $authors = substr($authors, 0, -2);
  $pub['Authors'] = $authors;
  
  // build the citation
  $pub['Citation'] = tripal_pub_create_citation($pub);
  
  $pub['raw'] = $pub_xml;
  return $pub;
}      

/*
 * 
 * 
 */
function tripal_pub_remote_search_AGL_get_subfield($xml) {
  $codes = array();
  while ($xml->read()) {    
    $sub_element = $xml->name;
    // when we've reached the end of the datafield element then break out of the while loop
    if ($xml->nodeType == XMLReader::END_ELEMENT and $sub_element == 'datafield') {
      return $codes; 
    }    
    // if inside the subfield element then get the code 
    if ($xml->nodeType == XMLReader::ELEMENT and $sub_element == 'subfield') {
      $code = $xml->getAttribute('code');
      $xml->read();
      $value = $xml->value;
      $codes[$code] = $value;
    }
  }
  return $codes;
}

/*
 * 
 * 
 */
function tripal_pub_remote_search_AGL_get_author($xml, $ind1) {
  $author = array();
  $codes = tripal_pub_remote_search_AGL_get_subfield($xml);
  foreach ($codes as $code => $value) {          
    switch ($code) {                
      case 'a':
        // remove any trailing commas
        $value = preg_replace('/,$/', '', $value);
        if ($ind1 == 0) { // Given Name is first
          $author['Given Name'] = $names[0];
        }
        if ($ind1 == 1) { // Surname is first          
          // split the parts of the name using a comma
          $names = explode(',', $value);
          $author['Surname'] = $names[0];
          $author['Given Name'] = ''; 
          unset($names[0]);
          foreach($names as $index => $name) {
            $author['Given Name'] .= $name . ' ';
          }
          $first_names = explode(' ', $author['Given Name']);
          $author['First Initials'] = '';
          foreach ($first_names as $index => $name) {
            $author['First Initials'] .= substr($name, 0, 1); 
          }
        }
        if ($ind1 == 3) { // A family name
          
        }
        break;
    }
  }
  return $author;
}