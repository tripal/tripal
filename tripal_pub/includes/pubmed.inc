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
function tripal_pub_remote_search_pubmed($search_array, $num_to_retrieve, $pager_id) {
  
  // convert the terms list provicded by the caller into a string with words
  // separated by a '+' symbol.
  $terms = '';
  if ($search_array['search_terms']) {
    $terms = '(' . implode("+", preg_split('/\s+/', trim($search_array['search_terms']))) . '[Title/Abstract]) AND ';
  }
  if ($search_array['author']) {
    $terms = "(" . $search_array . "[Author]) AND ";
  }
  $terms = substr($terms, 0, -4);
  
  $search_array['limit'] = $num_to_retrieve;

  // we want to get the list of pubs using the search terms but using a Drupal style pager
  $pubs = tripal_pager_callback('tripal_pub_remote_search_pubmed_range',  
    $num_to_retrieve, $pager_id, 'tripal_pub_remote_search_pubmed_count', $search_array);
 
  return $pubs;
}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns a count of
 * the dataset to be paged.
 */
function tripal_pub_remote_search_pubmed_count($search_array) {
  $terms = $search_array['search_terms'];
  $days = $search_array['days'];
  $limit = $search_array['limit'];
  
  $results = tripal_pub_remote_search_pubmed_search_init($terms, $limit, $days);
  $_SESSION['tripal_pub_pubmed_query'][$terms]['Count'] = $results['Count'];
  $_SESSION['tripal_pub_pubmed_query'][$terms]['WebEnv'] = $results['WebEnv'];
  $_SESSION['tripal_pub_pubmed_query'][$terms]['QueryKey'] = $results['QueryKey'];
  
  return $results['Count'];

}

/*
 * This function is used as the callback function when used with the
 * tripal_pager_callback function.  This function returns the results
 * within the specified range
 */
function tripal_pub_remote_search_pubmed_range($search_array, $start = 0, $limit = 10) {
  $terms = $search_array['search_terms'];
  $days = $search_array['days'];
  $limit = $search_array['limit'];
  
  // get the query_key and the web_env from the previous count query.
  $query_key = $_SESSION['tripal_pub_pubmed_query'][$terms]['QueryKey'];
  $web_env = $_SESSION['tripal_pub_pubmed_query'][$terms]['WebEnv'];
  
  // if this function has been called without calling the count function
  // then we need to do the query.
  if (!$query_key) {
    $results = tripal_pub_remote_search_pubmed_search_init($terms, $limit, $days);
    $_SESSION['tripal_pub_pubmed_query']['WebEnv'] = $results['WebEnv'];
    $_SESSION['tripal_pub_pubmed_query']['QueryKey'] = $results['QueryKey']; 
    $query_key =  $results['QueryKey'];
    $web_env = $results['WebEnv'];
  }

  // now get the list of PMIDs from the previous search
  $pmids_txt = tripal_pub_remote_search_pubmed_fetch($query_key, $web_env, 'uilist', 'text', $start, $limit);  
  
  // iterate through each PMID and get the publication record. This requires a new search and new fetch
  $pmids = explode("\n", trim($pmids_txt));
  $pubs = array();
  foreach ($pmids as $pmid) {
    // now retrieve the individual record
    $pub_xml = tripal_pub_remote_search_pubmed_fetch($query_key, $web_env, 'null', 'xml', 0, 1, array('id' => $pmid));
    $pub = tripal_pub_remote_search_pubmed_parse_pubxml($pub_xml);
    $pubs[] = $pub;    
  } 
  return $pubs;
}

/*
 * 
 */
function tripal_pub_remote_search_pubmed_search_init($terms, $retmax, $days = 0){
   
  // do a search for a single result so that we can establish a history, and get
  // the number of records. Once we have the number of records we can retrieve
  // those requested in the range.
  $query_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=Pubmed&retmax=$retmax&usehistory=y&term=$terms";
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
function tripal_pub_remote_search_pubmed_fetch($query_key, $web_env, $rettype = 'null', 
  $retmod = 'null', $start = 0, $limit = 10, $args = array()){

  // repeat the search performed previously (using WebEnv & QueryKey) to retrieve
  // the PMID's within the range specied.  The PMIDs will be returned as a text list
  $fetch_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?rettype=$rettype&retmode=$retmod&retstart=$start&retmax=$limit&db=Pubmed&query_key=$query_key&WebEnv=$web_env";
  //dpm($fetch_url);
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
  
  $rfh = fopen($fetch_url, "r");
  if (!$rfh) {
    drupal_set_message('Could not perform Pubmed query. Cannot connect to Entrez.', 'error');
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
function tripal_pub_remote_search_pubmed_parse_pubxml($pub_xml) {
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
          $xml->read(); // get the value for this element
          $pub['pub_accession'] = $xml->value;
          $pub['pub_database'] = 'PMID';
          break;        
        case 'Article':
          $pub_model = $xml->getAttribute('PubModel');
          $pub['publication_model'] = $pub_model;
          tripal_pub_remote_search_pubmed_parse_article($xml, $pub);
          break;
        case 'MedlineJournalInfo':
          tripal_pub_remote_search_pubmed_parse_medline_journal_info($xml, $pub);
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
  $pub['citation'] = $pub['author_list'] . 
    '. <a href="http://www.ncbi.nlm.nih.gov/pubmed/' . $pub['pub_accession'] . '" target="_blank">' . $pub['title'] .  '</a> ' .
    $pub['journal_iso_abbreviation']. '. ' . $pub['publication_date'];
  if ($pub['volume'] or $pub['issue']) {
    $pub['citation'] .= '; ';  
  }
  if ($pub['volume']) {
    $pub['citation'] .= $pub['volume'];
  }
  if ($pub['issue']) {
    $pub['citation'] .= '(' . $pub['issue'] . ')';
  }
  if ($pub['pages']) {
    $pub['citation'] .= ':' . $pub['pages'];
  }
  $pub['citation'] .= '. PubMed PMID: ' . $pub['pub_accession']; 
  
  $pub['xml'] = $pub_xml;
  return $pub;
}

/*
 * 
 */
function tripal_pub_remote_search_pubmed_parse_medline_journal_info($xml, &$pub) {
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
          $pub['journal_country'] = $xml->value;
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
function tripal_pub_remote_search_pubmed_parse_article($xml, &$pub) {
    
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
          tripal_pub_remote_search_pubmed_parse_journal($xml, $pub);
          break;
        case 'ArticleTitle':
          $xml->read();
          $pub['title'] = $xml->value;
          break;
        case 'Abstract':
          tripal_pub_remote_search_pubmed_parse_abstract($xml, $pub);
          break;
        case 'Pagination':
          tripal_pub_remote_search_pubmed_parse_pagination($xml, $pub);
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
          $pub['elocation'] = $elocation;
          break;        
        case 'Affiliation':
          // the affiliation tag at this level is meant solely for the first author
          $xml->read();
          $pub['authors'][0]['affiliation'] = $xml->value;
          break;
        case 'AuthorList':
          $complete = $xml->getAttribute('CompleteYN');
          tripal_pub_remote_search_pubmed_parse_authorlist($xml, $pub);
          break;
        case 'InvestigatorList':
          // TODO: perhaps handle this one day.  The investigator list is to list the names of people who 
          // are members of a collective or corporate group that is an author in the paper. 
          break;
        case 'Language':
          $xml->read();
          $lang_abbr = $xml->value;
          // there may be multiple languages so we store these in an array
          $pub['language'][] = tripal_pub_remote_search_get_language($lang_abbr);
          $pub['language_abbr'][] = $lang_abbr;
          break;
        case 'DataBankList':
          // TODO: handle this case
          break;
        case 'GrantList':
          // TODO: handle this case
          break;
        case 'PublicationTypeList':
          tripal_pub_remote_search_pubmed_parse_publication_type($xml, $pub);
          break;
        case 'VernacularTitle':
          $xml->read();
          $pub['vernacular_title'][] = $xml->value;;
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
function tripal_pub_remote_search_pubmed_parse_publication_type($xml, &$pub) {
  
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
          $pub['publication_type'][] = $xml->value;
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
function tripal_pub_remote_search_pubmed_parse_abstract($xml, &$pub) {
  $abstract = '';
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT and $element == 'Abstract') {
      // we've reached the </Abstract> element so return   
      $pub['abstract'] = $abstract;        
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
            $pub['structured_abstract_part'][][$label] = $xml->value;
            $abstract .= "<p><b>$label</b></br>" . $xml->value . '</p>';
          }
          else {
            $abstract .= '<p>' . $xml->value . '</p>';
          }
          break;
        case 'CopyrightInformation':
          $xml->read();
          $pub['copyright'] = $xml->value;
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
function tripal_pub_remote_search_pubmed_parse_pagination($xml, &$pub) {
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
            $pub['pages'] = $xml->value;
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
function tripal_pub_remote_search_pubmed_parse_journal($xml, &$pub) {
  
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
          tripal_pub_remote_search_pubmed_parse_journal_issue($xml, $pub);       
          break;        
        case 'Title': 
          $xml->read();
          $pub['journal_name'] = $xml->value;          
          break;
        case 'ISOAbbreviation': 
          $xml->read();
          $pub['journal_iso_abbreviation'] = $xml->value;         
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
function tripal_pub_remote_search_pubmed_parse_journal_issue($xml, &$pub) {
  
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
          $pub['volume'] = $xml->value;         
          break;
        case 'Issue':
          $xml->read();
          $pub['issue'] = $xml->value;           
          break;
        case 'PubDate':
          $date = tripal_pub_remote_search_pubmed_parse_date($xml, 'PubDate');
          $year = $date['year'];
          $month = $date['month'];
          $day = $date['day']; 
          $medline = $date['medline'];
           
          $pub['year'] = $year;            
          if ($month and $day and $year) {
            $pub['publication_date'] = "$year $month $day";
          }
          elseif ($month and !$day and $year) {
            $pub['publication_date'] = "$year $month";
          }
          elseif (!$month and !$day and $year) {
            $pub['publication_date'] = $year;
          } 
          elseif ($medline) {
            $pub['publication_date'] = $medline;
          }
          else {
            $pub['publication_date'] = "Date Unknown";
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
function tripal_pub_remote_search_pubmed_parse_date ($xml, $element_name) {
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
function tripal_pub_remote_search_pubmed_parse_authorlist($xml, &$pub) {
  $num_authors = 0;
  
  while ($xml->read()) {
    $element = $xml->name;
      
    if ($xml->nodeType == XMLReader::END_ELEMENT){
      // if we're at the </AuthorList> element then we're done with the article...
      if($element == 'AuthorList') {
        // build the author list before returning
        $author_list = '';
        foreach ($pub['authors'] as $author) {
          if ($author['valid'] == 'N') {
            // skip non-valid entries.  A non-valid entry should have 
            // a corresponding corrected entry so we can saftely skip it.
            continue;
          }
          if ($author['collective']) {
            $author_list .= $author['collective'] . ', ';
          }
          else {
            $author_list .= $author['surname'] . ' ' . $author['first_initials'] . ', '; 
          }             
        }
        $author_list = substr($author_list, 0, -2);
        $pub['author_list'] = $author_list;
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
          $pub['authors'][$num_authors]['valid'] = $valid;          
          break;
        case 'LastName':  
          $xml->read(); 
          $pub['authors'][$num_authors]['surname'] = $xml->value;         
          break;
        case 'ForeName': 
          $xml->read(); 
          $pub['authors'][$num_authors]['given_name'] = $xml->value;          
          break;
        case 'Initials': 
          $xml->read(); 
          $pub['authors'][$num_authors]['first_initials'] = $xml->value;          
          break;
        case 'Suffix': 
          $xml->read(); 
          $pub['authors'][$num_authors]['suffix'] = $xml->value;          
          break;
        case 'CollectiveName': 
          $xml->read(); 
          $pub['authors'][$num_authors]['collective'] = $xml->value;          
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
  return $languages[$lang_abbr];
}