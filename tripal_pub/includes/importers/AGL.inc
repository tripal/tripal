<?php
/**
 *
 */
function tripal_pub_remote_alter_form_AGL($form, $form_state) {

  $num_criteria = $form['num_criteria']['#default_value'];

  // So far we haven't been able to get AGL to filter results to only
  // include pubs by the XX number days in the past.  So, we will
  // change the 'days' element to be the year to query
  $form['days']['#title'] = t('Year');
  $form['days']['#description']  = t('Please enter a year to limit records by the year they were published, created or modified in the database.');

  // The Journal Name filter doesn't seem to work, so remove it
  for($i = 1; $i <= $num_criteria; $i++) {
    unset($form['criteria'][$i]["scope-$i"]['#options']['journal']);
  }
  return $form;
}
/**
 *
 */
function tripal_pub_remote_validate_form_AGL($form, $form_state) {
  $days =  trim($form_state['values']["days"]);
  $num_criteria = $form['num_criteria']['#default_value'];

  if ($days and !preg_match('/^\d\d\d\d$/', $days)) {
    form_set_error("days", "Please enter a four digit year.");
  }

  $num_ids = 0;
  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    if ($scope == 'id' and !preg_match('/^AGL:\d+$/', $search_terms)) {
      form_set_error("search_terms-$i", "The AGL accession be a numeric value, prefixed with 'AGL:' (e.g. AGL:3890740).");
    }
    if ($scope == 'id') {
      $num_ids++;
    }
    if($num_ids > 1) {
      form_set_error("search_terms-$i", "Unfortuantely, the AGL importer can only support a single accession at a time. Please remove the others.");
    }
  }
  return $form;
}
/**
 *
 */
function tripal_pub_remote_search_AGL($search_array, $num_to_retrieve, $pager_id) {
  // get some values from the serach array
  $num_criteria = $search_array['num_criteria'];
  $days = $search_array['days'];

  // set some defaults
  $search_array['limit'] = $num_to_retrieve;

  // To build the CCL search string we want to have a single entry for 'author', 'title', 'abstract'
  // or 'id', and also the corresponding 'not for each of those.
  // But the search form allows the user to have multiple rows of the same type. So, we will build the
  // search string separately for each category and it's negative category (if NOT is selected as the op)
  // and at the end we will put them together into a single search string.  We need to keep
  // track of the first entry of any category because it will not have an op (e.g. 'or' or 'and') but the
  // operation will be pushed out to separate the categories.  The op for any second or third instance of
  // the same category will be included within the search string for the catgory.
  $ccl = '';
  $title = '';
  $author = '';
  $abstract = '';
  $id = '';
  $any = '';
  $negate_title = '';
  $negate_author = '';
  $negate_abstract = '';
  $negate_id = '';
  $negate_any = '';
  $order = array();
  $first_abstract = 1;
  $first_author = 1;
  $first_title = 1;
  $first_id = 1;
  $first_any = 1;
  $first_negate_abstract = 1;
  $first_negate_author = 1;
  $first_negate_title = 1;
  $first_negate_id = 1;
  $first_negate_any = 1;
  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = trim($search_array['criteria'][$i]['search_terms']);
    $scope = $search_array['criteria'][$i]['scope'];
    $is_phrase = $search_array['criteria'][$i]['is_phrase'];
    $op = $search_array['criteria'][$i]['operation'];

    if ($op) {
      $op = strtolower($op);
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

    // if this is a 'not' operation then we want to change it to an
    // and
    $negate = '';
    if ($op == 'not') {
      $scope = "negate_$scope";
      $op = 'or';
    }
    $order[] = array('scope' => $scope, 'op' => $op);

    // build each category
    if ($scope == 'title') {
      if ($first_title) {
        $title .= "($search_terms) ";
        $first_title = 0;
      }
      else {
        $title .= "$op ($search_terms) ";
      }
    }
    if ($scope == 'negate_title') {
      if ($first_negate_title) {
        $negate_title .= "($search_terms) ";
        $first_negate_title = 0;
      }
      else {
        $negate_title .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'author') {
      if ($first_author) {
        $author .= "($search_terms) ";
        $first_author = 0;
      }
      else {
        $author .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'negate_author') {
      if ($first_negate_author) {
        $negate_author .= "($search_terms) ";
        $first_negate_author = 0;
      }
      else {
        $negate_author .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'abstract') {
      if ($first_abstract) {
        $abstract .= "($search_terms) ";
        $first_abstract = 0;
      }
      else {
        $abstract .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'negate_abstract') {
      if ($first_negate_abstract) {
        $negate_abstract .= "($search_terms) ";
        $first_negate_abstract = 0;
      }
      else {
        $negate_abstract .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'journal') {
      if ($first_journal) {
        $journal .= "($search_terms) ";
        $first_jounral = 0;
      }
      else {
        $journal .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'negate_journal') {
      if ($first_negate_journal) {
        $negate_journal .= "($search_terms) ";
        $first_negate_journal = 0;
      }
      else {
        $negate_journal .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'id') {
      if ($first_id) {
        $id .= "(" . preg_replace('/AGL:([^\s]*)/', '$1', $search_terms) . ") ";
        $first_id = 0;
      }
      else {
        $id .= "$op (" . preg_replace('/AGL:([^\s]*)/', '$1', $search_terms) . ") ";
      }
    }
    elseif ($scope == 'negate_id') {
      if ($first_negate_id) {
        $negate_id .= "(" . preg_replace('/AGL:([^\s]*)/', '$1', $search_terms) . ") ";
        $first_negate_id = 0;
      }
      else {
        $negate_id .= "$op (" . preg_replace('/AGL:([^\s]*)/', '$1', $search_terms) . ") ";
      }
    }
    elseif ($scope == 'any'){
      if ($first_any) {
        $any .= "($search_terms) ";
        $first_any = 0;
      }
      else {
        $any .= "$op ($search_terms) ";
      }
    }
    elseif ($scope == 'negate_any'){
      if ($first_negate_any) {
        $negate_any .= "($search_terms) ";
        $first_any = 0;
      }
      else {
        $negate_any .= "$op ($search_terms) ";
      }
    }
  }
  // now build the CCL string in order
  $abstract_done = 0;
  $author_done = 0;
  $journal_done = 0;
  $title_done = 0;
  $id_done = 0;
  $any_done = 0;
  $negate_abstract_done = 0;
  $negate_journal_done = 0;
  $negate_author_done = 0;
  $negate_title_done = 0;
  $negate_id_done = 0;
  $negate_any_done = 0;
  for ($i = 0; $i < count($order) ; $i++) {
    if ($order[$i]['scope'] == 'abstract' and !$abstract_done) {
      $op = $order[$i]['op'];
      $ccl .= "$op abstract=($abstract) ";
      $abstract_done = 1;
    }
    if ($order[$i]['scope'] == 'negate_abstract' and !$negate_abstract_done) {
      $ccl .= "not abstract=($negate_abstract) ";
      $negate_abstract_done = 1;
    }
    if ($order[$i]['scope'] == 'author' and !$author_done) {
      $op = $order[$i]['op'];
      $ccl .= "$op author=($author) ";
      $author_done = 1;
    }
    if ($order[$i]['scope'] == 'negate_author' and !$negate_author_done) {
      $ccl .= "not author=($negate_author) ";
      $negate_author_done = 1;
    }
    if ($order[$i]['scope'] == 'journal' and !$journal_done) {
      $op = $order[$i]['op'];
      $ccl .= "$op journal=($journal) ";
      $journal_done = 1;
    }
    if ($order[$i]['scope'] == 'negate_journal' and !$negate_journal_done) {
      $ccl .= "not author=($negate_journal) ";
      $negate_journal_done = 1;
    }
    if ($order[$i]['scope'] == 'id' and !$id_done) {
      $op = $order[$i]['op'];
      $ccl .= "$op id=($id) ";
      $id_done = 1;
    }
    if ($order[$i]['scope'] == 'negate_id' and !$negate_id_done) {
      $ccl .= "not id=($negate_id) ";
      $negate_id_done = 1;
    }
    if ($order[$i]['scope'] == 'title' and !$title_done) {
      $op = $order[$i]['op'];
      $ccl .= "$op title=($title) ";
      $title_done = 1;
    }
    if ($order[$i]['scope'] == 'negate_title' and !$negate_title_done) {
      $ccl .= "not title=($negate_title) ";
      $negate_title_done = 1;
    }
    if ($order[$i]['scope'] == 'any' and !$any_done) {
      $op = $order[$i]['op'];
      $ccl .= "$op ($any) ";
      $any_done = 1;
    }
    if ($order[$i]['scope'] == 'negate_any' and !$negate_any_done) {
      $ccl .= "not ($negate_any) ";
      $negate_any_done = 1;
    }
  }

  // for AGL the 'days' form element was converted to represent the year
  if ($days) {
    $ccl .= "and year=($days)";
  }

  // remove any preceeding 'and' or 'or'
  $ccl = preg_replace('/^\s*(and|or)/', '', $ccl);

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
    "year"     => "u=30 r=o", 
    "journal"  => "u=1033"
    );
    yaz_ccl_conf($yazc, $fields);

    if (!yaz_ccl_parse($yazc, $ccl, $cclresult)) {
      drupal_set_message('Error parsing search string: ' . $cclresult["errorstring"], "error");
      watchdog('tpub_import', 'Error: %errstr', array('%errstr' => $cclresult["errorstring"]), WATCHDOG_ERROR);
      return array();
    }
    $search_str = $cclresult["rpn"];

    $search_array['search_string'] = $search_str;

    // save the YAZ connection in the session for use by other functions
    $_SESSION['tripal_pub_AGL_query'][$search_str]['yaz_connection'] = $yazc;

    //dpm($search_array);
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
    watchdog('tpub_import', "ERROR preparing search at AGL: (%error_no) %error_msg",
              array('%error_no' => $error_no, '%error_msg' => $error_msg), WATCHDOG_ERROR);
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
    watchdog('tpub_import', "ERROR waiting on search at AGL: (%error_no) %error_msg",
              array('%error_no' => $error_no, '%error_msg' => $error_msg), WATCHDOG_ERROR);
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
    watchdog('tpub_import', "ERROR waiting on search at AGL: (%error_no) %error_msg",
              array('%error_no' => $error_no, '%error_msg' => $error_msg), WATCHDOG_ERROR);
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

  // we will set the default publication type as a journal article. The NAL
  // dataset doesn't specify an article type so we'll have to glean the type
  // from other information (e.g. series name has 'Proceedings' in it)
  $pub['Publication Type'][0] = 'Journal Article';

  if (!$pub_xml) {
    return $pub;
  }

  // read the XML and iterate through it.
  $xml = new XMLReader();
  $xml->xml(trim($pub_xml));
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
                $pub['Title'] = trim(preg_replace('/\.$/', '', $value));
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
            	case 'a':
            		if (preg_match('/Proceedings/i', $value)) {
            		  $pub['Series Name'] = preg_replace('/\.$/', '', $value);
            		  $pub['Publication Type'][0] = 'Conference Proceedings';
            		}
            		else {
            			$pub['Journal Name'] = preg_replace('/\.$/', '', $value);
            		}
                break;
              case 't':
                if (preg_match('/Proceedings/i', $value)) {
                  $pub['Series Name'] = preg_replace('/\.$/', '', $value);
                  $pub['Publication Type'][0] = 'Conference Proceedings';
                }
                $pub['Journal Name'] = preg_replace('/\.$/', '', $value);
                break;
              case 'g':
                $matches = array();
                if (preg_match('/^(\d\d\d\d)/', $value, $matches)) {
                  $pub['Publication Date'] = $matches[1];
                }
                elseif (preg_match('/(.*?)(\.|\s+)\s*(\d+),\s(\d\d\d\d)/', $value, $matches)) {
                  $year = $matches[4];
                  $month = $matches[1];
                  $day = $matches[3];
                  $pub['Publication Date'] = "$year $month $day";
                }
                elseif (preg_match('/\((.*?)(\.|\s+)(\d\d\d\d)\)/', $value, $matches)) {
                  $year = $matches[3];
                  $month = $matches[1];
                  $pub['Publication Date'] = "$year $month";
                }
                elseif (preg_match('/^(.*?) (\d\d\d\d)/', $value, $matches)) {
                  $year = $matches[2];
                  $month = $matches[1];
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
  if (is_array($pub['Author List'])) {
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
  }
  else {
    $pub['Authors'] = $pub['Author List'];	
  }

  // for Title, Abstract, Authors, convert the html entity and remove special unicode chars that are not meant for display
  $pub['Title'] = preg_replace( '/[\p{So}]/u', '', mb_convert_encoding($pub['Title'], 'UTF-8', 'HTML-ENTITIES'));
  if (key_exists('Abstract', $pub)) {
    $pub['Abstract'] =preg_replace( '/[\p{So}]/u', '',mb_convert_encoding($pub['Abstract'], 'UTF-8', 'HTML-ENTITIES'));
  }
  $newauths = array();
  foreach ($pub['Author List'] AS $auth) {
    foreach($auth AS $k => $v) {
      $auth[$k] = preg_replace( '/[\p{So}]/u', '',mb_convert_encoding($v, 'UTF-8', 'HTML-ENTITIES'));
    }
    array_push($newauths, $auth);
  }
  $pub['Author List'] = $newauths;

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
