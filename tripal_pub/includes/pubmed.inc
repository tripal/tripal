<?php 
/**
 * @file
 * Tripal Pub PubMed Interface
 *
 * @defgroup tripal_pub_pubmed PubMed Interface
 * @ingroup tripal_pub
 */

/**
 *
 */
function tripal_pub_remote_search_pubmed($terms_str, $num_to_retrieve, $pager_id) {

  // convert the terms list provicded by the caller into a string with words
  // separated by a '+' symbol.
  $search_terms = implode("+", preg_split('/\s+/', trim($terms_str)));

  // we want to get the list of pubs using the search terms but using a Drupal style pager
  $pubs = tripal_pager_callback('tripal_pub_remote_search_pubmed_range',
    'tripal_pub_remote_search_pubmed_count', $num_to_retrieve, $pager_id, $search_terms);
  
  if ($pubs) {
    foreach ($pubs as $pub) {
      /*
      $pmid = $output[$i];
  
      //aquiring the pubmed id from the pub table based on the uniquename
      $values = array( 'uniquename' => $pmid);
      $pubmed_id = tripal_core_chado_select('pub', array('pub_id'), $values); */
    }
  }
  return $pubs;
}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns a count of
 * the dataset to be paged.
 */
function tripal_pub_remote_search_pubmed_count($terms) {

  // do a quick query using the provided terms, set the session variables
  // so we can reuse this query and then return the total number of records.
  $results = tripal_pub_remote_search_pubmed_query($terms);
  $_SESSION['tripal_pub_pubmed_query']['WebEnv'] = $results['WebEnv'];
  $_SESSION['tripal_pub_pubmed_query']['QueryKey'] = $results['QueryKey'];
    
  return $total_records;
}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns the results
 * within the specified range
 */
function tripal_pub_remote_search_pubmed_range($terms, $start = 0, $limit = 10) {

  // get the query_key and the web_env from the previous count query.
  $query_key = $_SESSION['tripal_pub_pubmed_query']['QueryKey'];
  $web_env = $_SESSION['tripal_pub_pubmed_query']['WebEnv'];
     
  // repeat the search performed previously (using WebEnv & QueryKey) to retrieve
  // the PMID's within the range specied.  The PMIDs will be returned as a text list
  $pmids_txt = tripal_pub_remote_search_pubmed_fetch($terms, $query_key, $web_env, 'uilist', $start, $limit);  
  
  // iterate through each PMID and get the publication record. This requires a new search and new fetch
  $pmids = explode("\n", trim($pmids_txt));
  $pubs = array();
  foreach ($pmids as $pmid) {
    
    // first intialize the search for a single PMID. This will give us a new query key and Web env 
    $term = $pmid . "[uid]";    
    $query = tripal_pub_remote_search_pubmed_query($terms); 
    
    // second retrieve the individual record
    $pub_xml = tripal_pub_remote_search_pubmed_fetch($terms, $query['QueryKey'], $query['WebEnv'], 'xml', 0, 1); 
    $pubs[] = $pub_xml;
    
  } 
  return $pubs;
}

/*
 * 
 */
function tripal_pub_remote_search_pubmed_query($terms){
   
  // do a search for a single result so that we can establish a history, and get
  // the number of records. Once we have the number of records we can retrieve
  // those requested in the range.
  $query_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=Pubmed&retmax=1&usehistory=y&term=$terms";
  $rfh = fopen($query_url, "r");
  if (!$rfh) {
    drupal_set_message('Could not perform Pubmed query. Cannot connect to Entrez.', 'error');
    return 0;    
  }
  
  // retrieve the XML results
  $query_xml = '';
  while (!feof($rfh)) {
    $query_xml .= fread($rfh, 255);
  }
  fclose($rfh);
  $xml = new XMLReader();
  $xml->xml($query_xml);
  
  // iterate though the child nodes of the <eSearchResult> tag and get the count, history and query_id
  $result = array();
  while ($xml->read()) {
    if ($xml->nodeType == XMLReader::ELEMENT) {
      $element = $xml->name;
      $xml->read();
      $value = $xml->value;
      switch ($element) {
        case 'Count':
          $result['Count'] = $value;
          break;        
        case 'WebEnv':
          $result['WebEnv'] = $value;
          break;
        case 'QueryKey':
          $result['QueryKey'] = $value;
          break;
      }
    }
  }  
  return $result;
}

/*
 * 
 */
function tripal_pub_remote_search_pubmed_fetch($terms, $query_key, $web_env, $rettype = 'xml', $start = 0, $limit = 10){
  
  // repeat the search performed previously (using WebEnv & QueryKey) to retrieve
  // the PMID's within the range specied.  The PMIDs will be returned as a text list
  $fetch_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?rettype=$rettype&retmode=text&retstart=$start&retmax=$limit&db=Pubmed&query_key=$query_key&WebEnv=$web_env";
  $rfh = fopen($fetch_url, "r");
  $results = '';
  while (!feof($rfh)) {
    $results .= fread($rfh, 255);
  }  
  fclose($rfh);
  
  return $results;
}

/*
 * This function parses the XML containing details of a publication and
 * converts it into an associative array of where keys are Tripal Pub 
 * ontology terms and the values are extracted from the XML. The
 * XML should contain only a single publication record.
 */
function tripal_pub_remote_search_pubmed_parse_pubxml($pub_xml) {
  $pub = array();
  
  // read the XML and iterate through it.
  $xml = new XMLReader();
  $xml->xml($pub_xml);
  while ($xml->read()) {    
    $element = $xml->name;    
    if ($xml->nodeType == XMLReader::ELEMENT) {
      $xml->read();
      $value = $xml->value;
      switch ($element) {
        case 'PMID':
          $pub['pub_accession'] = $value;
          $pub['pub_database'] = 'PMID';
          break;        
        case 'Article':
          tripal_pub_remote_search_pubmed_parse_article($xml, $pub);
          break;
      }
    }
  }
  
  return $pub;
}

/*
 * 
 */
function tripal_pub_remote_search_pubmed_parse_article($xml, &$pub) {
    
  while ($xml->read()) {
    $element = $xml->name;  
    // if we're at the </Article> element then we're done with the article...
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element = 'Article') {
      return;  
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      $element = $xml->name;
      $xml->read();
      $value = $xml->value;
      switch ($element) {
        case 'Journal':
          tripal_pub_remote_search_pubmed_parse_journal($xml, $pub);
          break;
        case 'ArticleTitle':
          break;
        case 'AbstractText':
          break;
        case 'Affiliation':
          break;
        case 'AuthorList':
          tripal_pub_remote_search_pubmed_parse_authorlist($xml, $pub);
          break;
        case 'Language':
          break;
        case 'ArticleDate':
          break; 
        default:
          break;     
      }
    }
  }
}
/*
 * 
 */
function tripal_pub_remote_search_pubmed_parse_journal($xml, &$pub) {
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT){
      // if we're at the </AuthorList> element then we're done with the article...
      if($element = 'Journal') {
        return;
      }
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      $xml->read();
      $value = $xml->value;
      switch ($element) {
        case 'ISSN':          
          break;
        case 'Volume':          
          break;
        case 'Year':          
          break;
        case 'Title':          
          break;
        case 'ISOAbbreviation':          
          break;
        default:
          break;
      }
    }
  }
}
/*
 * 
 */
function tripal_pub_remote_search_pubmed_parse_authorlist($xml, &$pub) {
  $authors = array();
  $author = array();
  $author_list = '';
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT){
      // if we're at the </AuthorList> element then we're done with the article...
      if($element = 'Article') {
        return;
      }
      // if we're at the end </Author> element then we're done with the author and we can
      // start a new one.
      if($element = 'Author') {
        return;
      }  
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      $xml->read();
      $value = $xml->value;
      switch ($element) {
        case 'Author':          
          break;
        case 'LastName':          
          break;
        case 'ForeName':          
          break;
        case 'Initials':          
          break;
        default:
          break;
      }
    }
  }
}