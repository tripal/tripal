<?php
/**
 * @file
 * This file provides support for importing and parsing of results from the 
 * NCBI PubMed database.  The functions here are used by
 * both the publication importer setup form and the publication importer.
 * 
 */


/**
 * A hook for altering the publication importer form.  It Changes the 
 * 'Abstract' filter to be 'Abstract/Title'.
 *
 * @param $form
 *   The Drupal form array
 * @param $form_state
 *   The form state array
 * @param $num_criteria
 *   The number of criteria the user currently has added to the form
 *
 * @return
 *   The form (drupal form api)
 *
 * @ingroup tripal_pub
 */
function tripal_pub_remote_alter_form_PMID($form, $form_state, $num_criteria = 1) {
  // PubMed doesn't have an 'Abstract' field, so we need to convert the criteria
  // from 'Abstract' to 'Title/Abstract'
  for($i = 1; $i <= $num_criteria; $i++) {
    $form['themed_element']['criteria'][$i]["scope-$i"]['#options']['abstract'] = 'Abstract/Title';
  }

  return $form;
}

/**
 * A hook for providing additional validation of importer setup form.
 *
 * @param $form
 *   The Drupal form array
 * @param $form_state
 *   The form state array
 *  
 * @return
 *  The form (drupal form api)
 *
 * @ingroup tripal_pub
 */
function tripal_pub_remote_validate_form_PMID($form, $form_state) {
  $num_criteria = $form_state['values']['num_criteria'];

  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    if ($scope == 'id' and !preg_match('/^PMID:\d+$/', $search_terms)) {
      form_set_error("search_terms-$i", "The PubMed accession must be a numeric value, prefixed with 'PMID:' (e.g. PMID:23024789).");
    }
  }
  return $form;
}

/**
 * A hook for performing the search on the PubMed database.
 *
 * @param $search_array
 *   An array containing the serach criteria for the serach
 * @param $num_to_retrieve
 *   Indicates the maximum number of publications to retrieve from the remote
 *   database
 * @param $page
 *   Indicates the page to retrieve.  This corresponds to a paged table, where
 *   each page has $num_to_retrieve publications.
 *
 * @return
 *  An array of publications.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_remote_search_PMID($search_array, $num_to_retrieve, $page) {
  // convert the terms list provicded by the caller into a string with words
  // separated by a '+' symbol.
  $num_criteria = $search_array['num_criteria'];
  $days = $search_array['days'];

  $search_str = '';

  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = trim($search_array['criteria'][$i]['search_terms']);
    $scope = $search_array['criteria'][$i]['scope'];
    $is_phrase = $search_array['criteria'][$i]['is_phrase'];
    $op = $search_array['criteria'][$i]['operation'];

    if ($op) {
      $search_str .= "$op ";
    }

    // if this is phrase make sure the search terms are surrounded by quotes
    if ($is_phrase) {
      $search_str .= "(\"$search_terms\" |SCOPE|)";
    }
    // if this is not a phase then we want to separate each 'OR or 'AND' into a unique criteria
    else {
      $search_str .= "(";
      if (preg_match('/and/i', $search_terms)) {
        $elements = preg_split('/\s+and+\s/i', $search_terms);
        foreach ($elements as $element) {
          $search_str .= "($element |SCOPE|) AND ";
        }
        $search_str = substr($search_str, 0, -5); // remove trailing 'AND '
      }
      elseif (preg_match('/or/i', $search_terms)) {
        $elements = preg_split('/\s+or+\s/i', $search_terms);
        foreach ($elements as $element) {
          $search_str .= "($element |SCOPE|) OR ";
        }
        $search_str = substr($search_str, 0, -4); // remove trailing 'OR '
      }
      else {
        $search_str .= "($search_terms |SCOPE|)";
      }
      $search_str .= ')';
    }

    if ($scope == 'title') {
      $search_str = preg_replace('/\|SCOPE\|/', '[Title]', $search_str);
    }
    elseif ($scope == 'author') {
      $search_str = preg_replace('/\|SCOPE\|/', '[Author]', $search_str);
    }
    elseif ($scope == 'abstract') {
      $search_str = preg_replace('/\|SCOPE\|/', '[Title/Abstract]', $search_str);
    }
    elseif ($scope == 'journal') {
      $search_str = preg_replace('/\|SCOPE\|/', '[Journal]', $search_str);
    }
    elseif ($scope == 'id') {
      $search_str = preg_replace('/PMID:([^\s]*)/', '$1', $search_str);
      $search_str = preg_replace('/\|SCOPE\|/', '[Uid]', $search_str);
    }
    else {
      $search_str = preg_replace('/\|SCOPE\|/', '', $search_str);
    }
  }
  if ($days) {
    // get the date of the day suggested
    $past_timestamp = REQUEST_TIME - ($days * 86400);
    $past_date = getdate($past_timestamp);
    $search_str .= " AND (\"" . sprintf("%04d/%02d/%02d", $past_date['year'], $past_date['mon'], $past_date['mday']) . "\"[Date - Create] : \"3000\"[Date - Create]))";
  }

  // now initialize the query
  $results = tripal_pub_PMID_search_init($search_str, $num_to_retrieve);
  $total_records = $results['Count'];
  $query_key     = $results['QueryKey'];
  $web_env       = $results['WebEnv'];

  // initialize the pager
  $start = $page * $num_to_retrieve;

  // if we have no records then return an empty array
  if ($total_records == 0) {
    return array(
      'total_records' => $total_records,
      'search_str'    => $search_str,
      'pubs'          => array(),
    );
  }

  // now get the list of PMIDs from the initialized search
  $pmids_txt = tripal_pub_PMID_fetch($query_key, $web_env, 'uilist', 'text', $start, $num_to_retrieve);

  // iterate through each PMID and get the publication record. This requires a new search and new fetch
  $pmids = explode("\n", trim($pmids_txt));
  $pubs = array();
  foreach ($pmids as $pmid) {
    // now retrieve the individual record
    $pub_xml = tripal_pub_PMID_fetch($query_key, $web_env, 'null', 'xml', 0, 1, array('id' => $pmid));
    $pub     = tripal_pub_PMID_parse_pubxml($pub_xml);
    $pubs[]  = $pub;
  }
  return array(
    'total_records' => $total_records,
    'search_str'    => $search_str,
    'pubs'          => $pubs,
  );
}

/**
 * Initailizes a PubMed Search using a given search string
 *
 * @param $search_str
 *   The PubMed Search string
 * @param $retmax
 *   The maximum number of records to return
 *   
 * @return 
 *   An array containing the Count, WebEnv and QueryKey as return
 *   by PubMed's esearch utility
 *
 * @ingroup tripal_pub
 */
function tripal_pub_PMID_search_init($search_str, $retmax){

  // do a search for a single result so that we can establish a history, and get
  // the number of records. Once we have the number of records we can retrieve
  // those requested in the range.
  $query_url = "http://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" .
    "db=Pubmed" .
    "&retmax=$retmax" .
    "&usehistory=y".
    "&term=" . urlencode($search_str);

  //dpm($query_url);
  $rfh = fopen($query_url, "r");
  if (!$rfh) {
    drupal_set_message('Could not perform Pubmed query. Cannot connect to Entrez.', 'error');
    tripal_report_error('tripal_pubmed', TRIPAL_ERROR, "Could not perform Pubmed query. Cannot connect to Entrez.",
              array());
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

/**
 * Retreives from PubMed a set of publications from the
 * previously initiated query.
 *
 * @param $query_key
 *   The esearch QueryKey
 * @param $web_env
 *   The esearch WebEnv
 * @param $rettype
 *   The efetch return type 
 * @param $retmod
 *   The efetch return mode
 * @param $start
 *   The start of the range to retrieve
 * @param $limit
 *   The number of publications to retrieve
 * @param $args
 *   Any additional arguments to add the efetch query URL
 *   
 * @return
 *  An array containing the total_records in the dataaset, the search string
 *  and an array of the publications that were retreived.
 *
 * @ingroup tripal_pub
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
    tripal_report_error('tripal_pubmed', TRIPAL_ERROR, "Could not perform PubMed query: %fetch_url.",
              array('%fetch_url' => $fetch_url));
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

/**
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
 *
 * @param $pub_xml
 *  An XML string describing a single publication
 *
 * @return
 *  An array describing the publication
 *
 * @ingroup tripal_pub
 */
function tripal_pub_PMID_parse_pubxml($pub_xml) {
  $pub = array();

  if (!$pub_xml) {
    return $pub;
  }

  // read the XML and iterate through it.
  $xml = new XMLReader();
  $xml->xml(trim($pub_xml));
  while ($xml->read()) {
    $element = $xml->name;
    if ($xml->nodeType == XMLReader::ELEMENT) {

      switch ($element) {
        case 'ERROR':
          $xml->read(); // get the value for this element
          tripal_report_error('tripal_pubmed', TRIPAL_ERROR, "Error: %err", array('%err' => $xml->value));
          break;
        case 'PMID':
          // thre are multiple places where a PMID is present in the XML and
          // since this code does not descend into every branch of the XML tree
          // we will encounter many of them here.  Therefore, we only want the
          // PMID that we first encounter. If we already have the PMID we will
          // just skip it.  Examples of other PMIDs are in the articles that
          // cite this one.
          $xml->read(); // get the value for this element
          if(!array_key_exists('Publication Dbxref', $pub)) {
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
  $pub['Citation'] = tripal_pub_create_citation($pub);

  $pub['raw'] = $pub_xml;
  return $pub;
}

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about the Journal 
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about an article.
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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
          $pub['Title'] = trim(preg_replace('/\.$/', '', $xml->value));
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

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about a publication
 *
 * A full list of publication types can be found here:
 * http://www.nlm.nih.gov/mesh/pubtypes.html.
 *
 * The Tripal Pub ontology doesn't yet have terms for all of the
 * publication types so we store the value in the 'publication_type' term.
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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
          $value = $xml->value;
          
          $identifiers = array(
            'name' => $value,
            'cv_id' => array(
              'name' => 'tripal_pub',
            )
          );
          $options = array('case_insensitive_columns' => array('name'));
          $pub_cvterm = tripal_get_cvterm($identifiers, $options);
          if (!$pub_cvterm) {
            // see if this we can find the name using a synonym
            $identifiers = array(
              'synonym' => array(
                'name' => $value,
                'cv_name' => 'tripal_pub'
              )
            );
            $pub_cvterm = tripal_get_cvterm($identifiers, $options);
            if (!$pub_cvterm) {
              tripal_report_error('tripal_pubmed', TRIPAL_ERROR, 
                'Cannot find a valid vocabulary term for the publication type: "%term".',
                array('%term' => $value));
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

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about the abstract
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about pagination
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about a journal
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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
          $pub['Journal Abbreviation'] = $xml->value;
          break;
        default:
          break;
      }
    }
  }
}

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about a journal issue
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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
          $month = array_key_exists('month', $date) ? $date['month'] : '';
          $day = array_key_exists('day', $date) ? $date['day'] : '';
          $medline = array_key_exists('medline', $date) ? $date['medline'] : '';

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

/**
 * Parses the section from the XML returned from PubMed that contains
 * information regarding to dates
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
 */
function tripal_pub_PMID_parse_date($xml, $element_name) {
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
          $date['year'] = preg_replace('/^(\d{4}).*$/', '\1', $xml->value);
          $date['medline'] = $xml->value;
          break;
        default:
          break;
      }
    }
  }
}

/**
 * Parses the section from the XML returned from PubMed that contains
 * information about the author list for a publication
 *
 * @param $xml
 *   The XML to parse
 * @param $pub
 *   The publication object to which additional details will be added
 *
 * @ingroup tripal_pub
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
          if (array_key_exists('Collective', $author)) {
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

/**
 * Get the name of the language based on an abbreviation
 *
 * Language abbreviations were obtained here:
 * http://www.nlm.nih.gov/bsd/language_table.html
 *
 * @param $lang_abbr
 *   The abbreviation of the language to return
 *
 * @return
 *   The full name of the language
 *
 * @ingroup tripal_pub
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
