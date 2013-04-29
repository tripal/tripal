<?php 
/**
 * @file
 * Tripal Pub PubMed Interface
 *
 * @defgroup tripal_pub_PMID PubMed Interface
 * @ingroup tripal_pub
 */

/**
 *
 */
function tripal_pub_remote_search_PMID($search_array, $num_to_retrieve, $pager_id) {
  
  // convert the terms list provicded by the caller into a string with words
  // separated by a '+' symbol.
  $num_criteria = $search_array['num_criteria'];
  $days = $search_array['days'];

  $search_str = '';
  for ($i = 0; $i <= $num_criteria; $i++) {
    $search_terms = $search_array['criteria'][$i]['search_terms'];
    $scope = $search_array['criteria'][$i]['scope'];
    $op = $search_array['criteria'][$i]['operation'];
    
    if ($op) {
      $search_str .= "$op";
    }
    
    $search_str .= '(';
    
    if($scope == 'title') {
      $search_str .= '"' . implode("+", preg_split('/\s+/', trim($search_terms))) . '"[Title]';
    }    
    elseif($scope == 'author') {
      $search_str .= '"' . implode("+", preg_split('/\s+/', trim($search_terms))) . '"[Author]';
    }    
    elseif($scope == 'abstract') {
      $search_str .= '"' . implode("+", preg_split('/\s+/', trim($search_terms))) . '"[Title/Abstract]';
    }
    elseif($scope == 'id') {
      $search_str .= '' . implode("+", preg_split('/\s+/', trim($search_terms))) . '[Uid]';      
    }    
    $search_str .= ')'; 
  }  
  $search_array['limit'] = $num_to_retrieve;
  $search_array['search_string'] = $search_str;
    
  // we want to get the list of pubs using the search terms but using a Drupal style pager
  $pubs = tripal_pager_callback('tripal_pub_PMID_range',  $num_to_retrieve, $pager_id, 
    'tripal_pub_PMID_count', $search_array);
 
  return $pubs;
}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns a count of
 * the dataset to be paged.
 */
function tripal_pub_PMID_count($search_array) {
  $terms = $search_array['search_string'];
  $days  = $search_array['days'];
  $limit = $search_array['limit'];
  
  $results = tripal_pub_PMID_search_init($terms, $limit, $days);
  $_SESSION['tripal_pub_PMID_query'][$terms]['Count'] = $results['Count'];
  $_SESSION['tripal_pub_PMID_query'][$terms]['WebEnv'] = $results['WebEnv'];
  $_SESSION['tripal_pub_PMID_query'][$terms]['QueryKey'] = $results['QueryKey'];
   
  return $results['Count'];

}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns the results
 * within the specified range
 */
function tripal_pub_PMID_range($search_array, $start = 0, $limit = 10) {
  $terms = $search_array['search_string'];
  $days  = $search_array['days'];
  $limit = $search_array['limit'];
  
  $count = $_SESSION['tripal_pub_PMID_query'][$terms]['Count'];
  if ($count == 0) {
    return array();
  }
    
  // get the query_key and the web_env from the previous count query.
  $query_key = $_SESSION['tripal_pub_PMID_query'][$terms]['QueryKey'];
  $web_env   = $_SESSION['tripal_pub_PMID_query'][$terms]['WebEnv'];
  
  // if this function has been called without calling the count function
  // then we need to do the query.
  if (!$query_key) {
    $results = tripal_pub_PMID_search_init($terms, $limit, $days);
    $_SESSION['tripal_pub_PMID_query']['WebEnv'] = $results['WebEnv'];
    $_SESSION['tripal_pub_PMID_query']['QueryKey'] = $results['QueryKey']; 
    $query_key =  $results['QueryKey'];
    $web_env = $results['WebEnv'];
  }

  // now get the list of PMIDs from the previous search  
  $pmids_txt = tripal_pub_PMID_fetch($query_key, $web_env, 'uilist', 'text', $start, $limit);  
  
  // iterate through each PMID and get the publication record. This requires a new search and new fetch
  $pmids = explode("\n", trim($pmids_txt));
  $pubs = array();
  foreach ($pmids as $pmid) {
    // now retrieve the individual record
    $pub_xml = tripal_pub_PMID_fetch($query_key, $web_env, 'null', 'xml', 0, 1, array('id' => $pmid));
    $pub     = tripal_pub_PMID_parse_pubxml($pub_xml);
    $pubs[]  = $pub;    
  } 
  return $pubs;
}

/*
 * 
 */
function tripal_pub_PMID_search_init($terms, $retmax, $days = 0){
   
  // do a search for a single result so that we can establish a history, and get
  // the number of records. Once we have the number of records we can retrieve
  // those requested in the range.
  $query_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" .
    "db=Pubmed" .
    "&retmax=$retmax" .
    "&usehistory=y".
    "&term=$terms";
  if($days) {
    $query_url .= "&reldate=$days&datetype=edat";
  }
  //dpm($query_url);
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
  //dpm("<pre>$query_xml</pre>");
  $xml = new XMLReader();
  $xml->xml($query_xml);
  
  // iterate though the child nodes of the <eSearchResult> tag and get the count, history and query_id
  $result = array();
  while ($xml->read()) {
    $element = $xml->name;
  
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'WebEnv') {
      // we've read as much as we need. If we go too much further our counts 
      // will get messed up by other 'Count' elements.  so we're done.
      break;
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      
      switch ($element) {
        case 'Count':
          $xml->read();
          $result['Count'] = $xml->value;
          break;        
        case 'WebEnv':
          $xml->read();
          $result['WebEnv'] = $xml->value;
          break;
        case 'QueryKey':
          $xml->read();
          $result['QueryKey'] = $xml->value;
          break;
      }
    }
  }  
  return $result;
}

/*
 * 
 */
function tripal_pub_PMID_fetch($query_key, $web_env, $rettype = 'null', 
  $retmod = 'null', $start = 0, $limit = 10, $args = array()){

  // repeat the search performed previously (using WebEnv & QueryKey) to retrieve
  // the PMID's within the range specied.  The PMIDs will be returned as a text list
  $fetch_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?".
    "rettype=$rettype" .
    "&retmode=$retmod" . 
    "&retstart=$start" .
    "&retmax=$limit" .
    "&db=Pubmed" .
    "&query_key=$query_key".
    "&WebEnv=$web_env";
  
  foreach ($args as $key => $value) {
    if(is_array($value)) {
      $fetch_url .= "&$key=";
      foreach ($value as $item) {
        $fetch_url .= "$item,";
      }
      $fetch_url = substr($fetch_url, 0, -1); // remove trailing comma  
    }
    else {
      $fetch_url .= "&$key=$value";
    }
  }
  //dpm($fetch_url);
  $rfh = fopen($fetch_url, "r");
  if (!$rfh) {
    drupal_set_message('ERROR: Could not perform PubMed query.', 'error');
    return '';    
  }
  $results = '';
  if($rfh) {
    while (!feof($rfh)) {
      $results .= fread($rfh, 255);
    }  
    fclose($rfh);
  }
  
  return $results;
}

/*
 * This function parses the XML containing details of a publication and
 * converts it into an associative array of where keys are Tripal Pub 
 * ontology terms and the values are extracted from the XML. The
 * XML should contain only a single publication record.  
 * 
 * Information about the valid elements in the PubMed XML can be found here:
 * http://www.nlm.nih.gov/bsd/licensee/elements_descriptions.html
 * 
 * Information about PubMed's citation format can be found here
 * http://www.nlm.nih.gov/bsd/policy/cit_format.html
 */
function tripal_pub_PMID_parse_pubxml($pub_xml) {
  $pub = array();
  
  if (!$pub_xml) {
    return $pub;
  }
  
  // read the XML and iterate through it.
  $xml = new XMLReader();
  $xml->xml($pub_xml);
  while ($xml->read()) {    
    $element = $xml->name;    
    if ($xml->nodeType == XMLReader::ELEMENT) {
      
      switch ($element) {
        case 'PMID':
          // thre are multiple places where a PMID is present in the XML and
          // since this code does not descend into every branch of the XML tree
          // we will encounter many of them here.  Therefore, we only want the
          // PMID that we first encounter. If we already have the PMID we will 
          // just skip it.  Examples of other PMIDs are in the articles that
          // cite this one.
          $xml->read(); // get the value for this element
          if(!$pub['Publication Dbxref']) {
            $pub['Publication Dbxref'] = 'PMID:' . $xml->value;
          }
          break;        
        case 'Article':
          $pub_model = $xml->getAttribute('PubModel');
          $pub['Publication Model'] = $pub_model;
          tripal_pub_PMID_parse_article($xml, $pub);
          break;
        case 'MedlineJournalInfo':
          tripal_pub_PMID_parse_medline_journal_info($xml, $pub);
          break;
        case 'ChemicalList':
          // TODO: handle this
          break;
        case 'SupplMeshList':
          // TODO: meant for protocol list
          break;
        case 'CitationSubset':
          // TODO: not sure this is needed.         
          break;
        case 'CommentsCorrections':
          // TODO: handle this
          break;
        case 'GeneSymbolList':
          // TODO: handle this
          break;
        case 'MeshHeadingList':
          // TODO: Medical subject headings
          break;
        case 'NumberOfReferences':
          // TODO: not sure we should keep this as it changes frequently.
          break;
        case 'PersonalNameSubjectList':
          // TODO: for works about an individual or with biographical note/obituary.
          break;
        case 'OtherID':
          // TODO: ID's from another NLM partner.
          break;
        case 'OtherAbstract':
          // TODO: when the journal does not contain an abstract for the publication.
          break;
        case 'KeywordList':
          // TODO: handle this
          break;
        case 'InvestigatorList':
          // TODO: personal names of individuals who are not authors (can be used with collection)
          break;
        case 'GeneralNote':
          // TODO: handle this
          break;
        case 'DeleteCitation':
          // TODO: need to know how to handle this
          break;          
        default:
          break;
      }
    }
  }
  $pub['Citation'] = $pub['Authors'] . '. ' . $pub['Title'] .  '. ' .
    $pub['Journal ISO Abbreviation'] . '. ' . $pub['Publication Date'];
  if ($pub['Volume'] or $pub['Issue']) {
    $pub['Citation'] .= '; ';  
  }
  if ($pub['Volume']) {
    $pub['Citation'] .= $pub['Volume'];
  }
  if ($pub['Issue']) {
    $pub['Citation'] .= '(' . $pub['Issue'] . ')';
  }
  if ($pub['Pages']) {
    $pub['Citation'] .= ':' . $pub['Pages'];
  }
  
  $pub['xml'] = $pub_xml;
  return $pub;
}

/*
 * 
 */
function tripal_pub_PMID_parse_medline_journal_info($xml, &$pub) {
  while ($xml->read()) {
    // get this element name
    $element = $xml->name;     
        
    // if we're at the </Article> element then we're done with the article...
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'MedlineJournalInfo') {
      return;  
    }
    if ($xml->nodeType == XMLReader::ELEMENT) { 
      switch ($element) {
        case 'Country':
          // the place of publication of the journal
          $xml->read();
          $pub['Journal Country'] = $xml->value;
          break;
        case 'MedlineTA':
          // TODO: not sure how this is different from ISOAbbreviation
          break;
        case 'NlmUniqueID':
          // TODO: the journal's unique ID in medline
          break;
        case 'ISSNLinking':
          // TODO: not sure how this is different from ISSN
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
function tripal_pub_PMID_parse_article($xml, &$pub) {
    
  while ($xml->read()) {
    // get this element name
    $element = $xml->name;     
        
    // if we're at the </Article> element then we're done with the article...
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'Article') {
      return;  
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {    
      switch ($element) {
        case 'Journal':
          tripal_pub_PMID_parse_journal($xml, $pub);
          break;
        case 'ArticleTitle':
          $xml->read();
          // remoave any trailing period from the title
          $pub['Title'] = preg_replace('/\.$/', '', $xml->value); 
          break;
        case 'Abstract':
          tripal_pub_PMID_parse_abstract($xml, $pub);
          break;
        case 'Pagination':
          tripal_pub_PMID_parse_pagination($xml, $pub);
          break;  
        case 'ELocationID':
          $type = $xml->getAttribute('EIdType');
          $valid = $xml->getAttribute('ValidYN');
          $xml->read();
          $elocation = $xml->value;
          if ($type == 'doi' and $valid == 'Y') {
            $pub['DOI'] = $elocation;
          }
          if ($type == 'pii' and $valid == 'Y') {
            $pub['PII'] = $elocation;
          }
          $pub['Elocation'] = $elocation;
          break;        
        case 'Affiliation':
          // the affiliation tag at this level is meant solely for the first author
          $xml->read();
          $pub['Author List'][0]['Affiliation'] = $xml->value;
          break;
        case 'AuthorList':
          $complete = $xml->getAttribute('CompleteYN');
          tripal_pub_PMID_parse_authorlist($xml, $pub);
          break;
        case 'InvestigatorList':
          // TODO: perhaps handle this one day.  The investigator list is to list the names of people who 
          // are members of a collective or corporate group that is an author in the paper. 
          break;
        case 'Language':
          $xml->read();
          $lang_abbr = $xml->value;
          // there may be multiple languages so we store these in an array
          $pub['Language'][] = tripal_pub_remote_search_get_language($lang_abbr);
          $pub['Language Abbr'][] = $lang_abbr;
          break;
        case 'DataBankList':
          // TODO: handle this case
          break;
        case 'GrantList':
          // TODO: handle this case
          break;
        case 'PublicationTypeList':
          tripal_pub_PMID_parse_publication_type($xml, $pub);
          break;
        case 'VernacularTitle':
          $xml->read();
          $pub['Vernacular Title'][] = $xml->value;
          break;
        case 'ArticleDate': 
          // TODO: figure out what to do with this element. We already have the
          // published date in the <PubDate> field, but this date should be in numeric
          // form and may have more information.         
          break;
        default:
          break;     
      }
    }
  }
}
/*
 * A full list of publication types can be found here:
 * http://www.nlm.nih.gov/mesh/pubtypes.html.
 * 
 * The Tripal Pub ontology doesn't yet have terms for all of the 
 * publication types so we store the value in the 'publication_type' term.
 */
function tripal_pub_PMID_parse_publication_type($xml, &$pub) {
  
  while ($xml->read()) {
    $element = $xml->name;    
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'PublicationTypeList') {
      // we've reached the </PublicationTypeList> element so we're done.
      return;
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      switch ($element) {
        case 'PublicationType':  
          $xml->read();
          $pub_cvterm = tripal_cv_get_cvterm_by_name($xml->value, NULL, 'tripal_pub');
          if (!$pub_cvterm) {
            // see if this we can find the name using a synonym
            $pub_cvterm = tripal_cv_get_cvterm_by_synonym($xml->value, NULL, 'tripal_pub');
            if (!$pub_cvterm) {
              watchdog('tpub_pubmed', 'Cannot find a valid vocabulary term for the publication type: "%term".', 
                array('%term' => $xml->value), WATCHDOG_ERROR);
            }
          }
          else {
            $pub['Publication Type'][] = $pub_cvterm->name;
          }            
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
function tripal_pub_PMID_parse_abstract($xml, &$pub) {
  $abstract = '';
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'Abstract') {
      // we've reached the </Abstract> element so return   
      $pub['Abstract'] = $abstract;        
      return;
    }
    // the abstract text can be just a singe paragraph or be broken into multiple
    // abstract texts for structured abstracts.  Here we will just combine then 
    // into a single element in the order that they arrive in HTML format
    if ($xml->nodeType == XMLReader::ELEMENT) {
      switch ($element) {
        case 'AbstractText':
          $label = $xml->getAttribute('Label');
          $xml->read();
          if ($label) { 
            $part = "<p><b>$label</b></br>" . $xml->value . '</p>';           
            $abstract .= $part;
            $pub['Structured Abstract Part'][] = $part;
          }
          else {
            $abstract .= '<p>' . $xml->value . '</p>';
          }
          break;
        case 'CopyrightInformation':
          $xml->read();
          $pub['Copyright'] = $xml->value;
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
function tripal_pub_PMID_parse_pagination($xml, &$pub) {
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'Pagination') {
      // we've reached the </Pagination> element so we're done.
      return;
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      switch ($element) {
        case 'MedlinePgn':  
          $xml->read();
          if(trim($xml->value)) {
            $pub['Pages'] = $xml->value;
          }
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
function tripal_pub_PMID_parse_journal($xml, &$pub) {
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'Journal') {
      return;
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      switch ($element) {
        case 'ISSN':  
          $issn_type = $xml->getAttribute('IssnType');          
          $xml->read();
          $issn = $xml->value;
          $pub['ISSN'] = $issn;
          if ($issn_type == 'Electronic') {            
            $pub['eISSN'] = $issn;  
          }
          if ($issn_type == 'Print') {            
            $pub['pISSN'] = $issn;  
          }                 
          break;
        case 'JournalIssue':   
          // valid values of cited_medium are 'Internet' and 'Print'
          $cited_medium = $xml->getAttribute('CitedMedium');                    
          tripal_pub_PMID_parse_journal_issue($xml, $pub);       
          break;        
        case 'Title': 
          $xml->read();
          $pub['Journal Name'] = $xml->value;          
          break;
        case 'ISOAbbreviation': 
          $xml->read();
          $pub['Journal ISO Abbreviation'] = $xml->value;         
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
function tripal_pub_PMID_parse_journal_issue($xml, &$pub) {
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'JournalIssue'){
      // if we're at the </JournalIssue> element then we're done 
      return;
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      switch ($element) {
        case 'Volume': 
          $xml->read();
          $pub['Volume'] = $xml->value;         
          break;
        case 'Issue':
          $xml->read();
          $pub['Issue'] = $xml->value;           
          break;
        case 'PubDate':
          $date = tripal_pub_PMID_parse_date($xml, 'PubDate');
          $year = $date['year'];
          $month = $date['month'];
          $day = $date['day']; 
          $medline = $date['medline'];
           
          $pub['Year'] = $year;            
          if ($month and $day and $year) {
            $pub['Publication Date'] = "$year $month $day";
          }
          elseif ($month and !$day and $year) {
            $pub['Publication Date'] = "$year $month";
          }
          elseif (!$month and !$day and $year) {
            $pub['Publication Date'] = $year;
          } 
          elseif ($medline) {
            $pub['Publication Date'] = $medline;
          }
          else {
            $pub['Publication Date'] = "Date Unknown";
          }             
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
function tripal_pub_PMID_parse_date ($xml, $element_name) {
  $date = array();
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == $element_name){
      // if we're at the </$element_name> then we're done 
      return $date;
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {           
      switch ($element) {
        case 'Year':
          $xml->read(); 
          $date['year'] = $xml->value;         
          break;
        case 'Month':
          $xml->read(); 
          $month = 
          $date['month'] = $xml->value;          
          break;
        case 'Day':
          $xml->read(); 
          $date['day'] = $xml->value;          
          break;
        case 'MedlineDate':
          // the medline date is when the date cannot be broken into distinct month day year.
          $xml->read(); 
          $date['medline'] = $xml->value;          
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
function tripal_pub_PMID_parse_authorlist($xml, &$pub) {
  $num_authors = 0;
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT){
      // if we're at the </AuthorList> element then we're done with the article...
      if($element == 'AuthorList') {
        // build the author list before returning
        $authors = '';
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
        return;
      }
      // if we're at the end </Author> element then we're done with the author and we can
      // start a new one.
      if($element == 'Author') {
        $num_authors++;
      }  
    }
    if ($xml->nodeType == XMLReader::ELEMENT) {
      switch ($element) {
        case 'Author':
          $valid = $xml->getAttribute('ValidYN');
          $pub['Author List'][$num_authors]['valid'] = $valid;          
          break;
        case 'LastName':  
          $xml->read(); 
          $pub['Author List'][$num_authors]['Surname'] = $xml->value;         
          break;
        case 'ForeName': 
          $xml->read(); 
          $pub['Author List'][$num_authors]['Given Name'] = $xml->value;          
          break;
        case 'Initials': 
          $xml->read(); 
          $pub['Author List'][$num_authors]['First Initials'] = $xml->value;          
          break;
        case 'Suffix': 
          $xml->read(); 
          $pub['Author List'][$num_authors]['Suffix'] = $xml->value;          
          break;
        case 'CollectiveName': 
          $xml->read(); 
          $pub['Author List'][$num_authors]['Collective'] = $xml->value;          
          break;
        case 'Identifier': 
          // according to the specification, this element is not yet used.                  
          break;
        default:
          break;
      }
    }
  }
}

/*
 * Language abbreviations were obtained here: 
 * http://www.nlm.nih.gov/bsd/language_table.html
 */
function tripal_pub_remote_search_get_language($lang_abbr) {
  $languages = array(
    'afr' => 'Afrikaans',
    'alb' => 'Albanian',
    'amh' => 'Amharic',
    'ara' => 'Arabic',
    'arm' => 'Armenian',
    'aze' => 'Azerbaijani',
    'ben' => 'Bengali',
    'bos' => 'Bosnian',
    'bul' => 'Bulgarian',
    'cat' => 'Catalan',
    'chi' => 'Chinese',
    'cze' => 'Czech',
    'dan' => 'Danish',
    'dut' => 'Dutch',
    'eng' => 'English',
    'epo' => 'Esperanto',
    'est' => 'Estonian',
    'fin' => 'Finnish',
    'fre' => 'French',
    'geo' => 'Georgian',
    'ger' => 'German',
    'gla' => 'Scottish Gaelic',
    'gre' => 'Greek, Modern',
    'heb' => 'Hebrew',
    'hin' => 'Hindi',
    'hrv' => 'Croatian',
    'hun' => 'Hungarian',
    'ice' => 'Icelandic',
    'ind' => 'Indonesian',
    'ita' => 'Italian',
    'jpn' => 'Japanese',
    'kin' => 'Kinyarwanda',
    'kor' => 'Korean',
    'lat' => 'Latin',
    'lav' => 'Latvian',
    'lit' => 'Lithuanian',
    'mac' => 'Macedonian',
    'mal' => 'Malayalam',
    'mao' => 'Maori',
    'may' => 'Malay',
    'mul' => 'Multiple languages',
    'nor' => 'Norwegian',
    'per' => 'Persian',
    'pol' => 'Polish',
    'por' => 'Portuguese',
    'pus' => 'Pushto',
    'rum' => 'Romanian, Rumanian, Moldovan',
    'rus' => 'Russian',
    'san' => 'Sanskrit',
    'slo' => 'Slovak',
    'slv' => 'Slovenian',
    'spa' => 'Spanish',
    'srp' => 'Serbian',
    'swe' => 'Swedish',
    'tha' => 'Thai',
    'tur' => 'Turkish',
    'ukr' => 'Ukrainian',
    'und' => 'Undetermined',
    'urd' => 'Urdu',
    'vie' => 'Vietnamese',
    'wel' => 'Welsh',
  );
  return $languages[strtolower($lang_abbr)];
}