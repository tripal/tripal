<?php

namespace Drupal\tripal\Plugin\TripalPubLibrary;

use Drupal\tripal\TripalPubLibrary\TripalPubLibraryBase;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * PubMed publication parser
 *
 *  @TripalPubLibrary(
 *    id = "tripal_pub_library_pubmed",
 *    label = @Translation("NIH PubMed database"),
 *    description = @Translation("Retrieves and parses publication data from the NIH PubMed database"),
 *  )
 */
class TripalPubLibraryPubmed extends TripalPubLibraryBase {
  public function formSubmit($form, &$form_state) {
    // DUMMY function from inheritance so it had to be kept.
    // The form_submit function which is called by TripalPubLibrary
    // is needed to receive and process the criteria data. See below.
  }

  /** 
   * Plugin specific form submit to add form values for example to criteria array
   * The criteria array eventually gets serialized and stored in the tripal_pub_import
   * database table. (This code gets called from ChadoNewPublicationForm)
   */
  public function form_submit($form, $form_state, &$criteria) {
    $user_input = $form_state->getUserInput();
    $criteria['days'] = $user_input['days'];
  }

  /**
   * Adds plugin specific form items and returns the $form array
   */
  public function form($form, &$form_state) {
    // Add form elements specific to this parser.
    $api_key_description = t('Tripal imports publications using NCBI\'s ')
      . Link::fromTextAndUrl('EUtils API',
          Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25500/'))->toString()
      . t(', which limits users and programs to a maximum of 3 requests per second without an API key. '
          . 'However, NCBI allows users and programs to an increased maximum of 10 requests per second if '
          . 'they provide a valid API key. This is particularly useful in speeding up large publication imports. '
          . 'For more information on NCBI API keys, please ')
      . Link::fromTextAndUrl(t('see here'),
          Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25497/#chapter2.API_Keys', [
            'attributes' => [
              'target' => 'blank',
            ]]))->toString()
      . '.';

    $form['pub_library']['ncbi_api_key'] = [
      '#title' => t('(Optional) NCBI API key:'),
      '#type' => 'textfield',
      '#description' => $api_key_description,
      '#required' => FALSE,
      //to-do add ajax callback to populate?
      '#size' => 20,
    ];

    $form['pub_library']['days'] = [
      '#title' => t('Days since record modified'),
      '#type' => 'textfield',
      '#description' => t('Limit the search to include pubs that have been added no more than this many days before today'),
      '#required' => FALSE,
      '#size' => 5,
    ];
    return $form;
  }

  public function formValidate($form, &$form_state) {
    // Perform any form validations necessary with the form data
  }


  /**
   * More documentation can be found in TripalPubLibraryInterface
   */
  public function run(int $query_id) {
    // public connection is already defined due to dependency injection happening on TripalPubLibraryBase
    $row = $this->public->select('tripal_pub_library_query', 'tpi')
    ->fields('tpi')
    ->condition('pub_library_query_id', $query_id, '=')
    ->execute()
    ->fetchObject();
    // Get the criteria column which has serialized data, so unserialize it into $query variable
    $query = unserialize($row->criteria);

    // Go through all results until pubs is empty
    $page_results = $this->retrieve($query);
    // print_r($page_results);
    // print_r(count($page_results['pubs']));
    $publications = [];
    if (count($page_results['pubs']) != 0) {
      $publications = array_merge($publications, $page_results['pubs']);
    }
    return $publications;
  }

  /**
   * More documentation can be found in TripalPubLibraryInterface
   */
  public function retrieve(array $query, int $limit = 10, int $page = 0) {
    $results = NULL;
    try {
      $results = $this->remoteSearchPMID($query, $limit, $page);
    }
    catch (\Exception $ex) {

    }
    return $results;
  }

  
  /**
   * A function for performing the search on the PubMed database.
   *
   * @param $search_array
   *   An array containing the search criteria for the search
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
  public function remoteSearchPMID($search_array, $num_to_retrieve, $page, $row_mode = 1) {
    // convert the terms list provided by the caller into a string with words
    // separated by a '+' symbol.
    $num_criteria = $search_array['num_criteria'];
    $days = NULL;
    if (isset($search_array['days'])) {
      $days = $search_array['days'];
    }
  
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
        if (preg_match('/\s+and+\s/i', $search_terms)) {
          $elements = preg_split('/\s+and+\s/i', $search_terms);
          foreach ($elements as $element) {
            $search_str .= "($element |SCOPE|) AND ";
          }
          $search_str = substr($search_str, 0, -5); // remove trailing 'AND '
        }
        elseif (preg_match('/\s+or+\s/i', $search_terms)) {
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
      $past_timestamp = time() - ($days * 86400);
      $past_date = getdate($past_timestamp);
      $search_str .= " AND (\"" . sprintf("%04d/%02d/%02d", $past_date['year'], $past_date['mon'], $past_date['mday']) . "\"[Date - Create] : \"3000\"[Date - Create]))";
    }
  
    // now initialize the query
    $results = $this->pmidSearchInit($search_str, $num_to_retrieve);
    $total_records = $results['Count'];
    $query_key = $results['QueryKey'];
    $web_env = $results['WebEnv'];
  
    // initialize the pager
    $start = $page * $num_to_retrieve;
 
    // if we have no records then return an empty array
    if ($total_records == 0) {
      return [
        'total_records' => $total_records,
        'search_str' => $search_str,
        'pubs' => [],
      ];
    }
    // now get the list of PMIDs from the initialized search
    $pmids_txt = $this->pmidFetch($query_key, $web_env, 'uilist', 'text', $start, $num_to_retrieve);
  
    // iterate through each PMID and get the publication record. This requires a new search and new fetch
    $pmids = explode("\n", trim($pmids_txt));
    $pubs = [];
    foreach ($pmids as $pmid) {
      // now retrieve the individual record
      $pub_xml = $this->pmidFetch($query_key, $web_env, 'null', 'xml', 0, 1, ['id' => $pmid]);
      $pub = $this->parse($pub_xml);
      $pubs[] = $pub;
    }
    return [
      'total_records' => $total_records,
      'search_str' => $search_str,
      'pubs' => $pubs,
    ];
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
  private function pmidSearchInit($search_str, $retmax) {
  
    // do a search for a single result so that we can establish a history, and get
    // the number of records. Once we have the number of records we can retrieve
    // those requested in the range.
    $query_url = "https://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" .
      "db=Pubmed" .
      "&retmax=$retmax" .
      "&usehistory=y" .
      "&term=" . urlencode($search_str);
  
    $api_key = \Drupal::state()->get('tripal_pub_importer_ncbi_api_key', NULL);
    $sleep_time = 333334;
    if (!empty($api_key)) {
      $query_url .= "&api_key=" . $api_key;
      $sleep_time = 100000;
    }
  
    usleep($sleep_time);  // 1/3 of a second delay, NCBI limits requests to 3 / second without API key
    $rfh = fopen($query_url, "r");
    if (!$rfh) {
      \Drupal::messenger()->addMessage('Could not perform Pubmed query. Cannot connect to Entrez.', 'error');
      \Drupal::service('tripal.logger')->error("Could not perform Pubmed query. Cannot connect to Entrez.");
      return 0;
    }
  
    // retrieve the XML results
    $query_xml = '';
    while (!feof($rfh)) {
      $query_xml .= fread($rfh, 255);
    }
    fclose($rfh);
    $xml = new \XMLReader();
    $xml->xml($query_xml);
  
    // iterate though the child nodes of the <eSearchResult> tag and get the count, history and query_id
    $result = [];
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'WebEnv') {
        // we've read as much as we need. If we go too much further our counts
        // will get messed up by other 'Count' elements.  so we're done.
        break;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
  
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
   * Retrieves from PubMed a set of publications from the
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
   *  An array containing the total_records in the dataset, the search string
   *  and an array of the publications that were retrieved.
   *
   * @ingroup tripal_pub
   */
  private function pmidFetch($query_key, $web_env, $rettype = 'null',
                                 $retmod = 'null', $start = 0, $limit = 10, $args = []) {
  
    // repeat the search performed previously (using WebEnv & QueryKey) to retrieve
    // the PMID's within the range specied.  The PMIDs will be returned as a text list
    $fetch_url = "https://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?" .
      "rettype=$rettype" .
      "&retmode=$retmod" .
      "&retstart=$start" .
      "&retmax=$limit" .
      "&db=Pubmed" .
      "&query_key=$query_key" .
      "&WebEnv=$web_env";
  
    $api_key = \Drupal::state()->get('tripal_pub_importer_ncbi_api_key', NULL);
    $sleep_time = 333334;
    if (!empty($api_key)) {
      $fetch_url .= "&api_key=" . $api_key;
      $sleep_time = 100000;
    }
  
    foreach ($args as $key => $value) {
      if (is_array($value)) {
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
    usleep($sleep_time);  // 1/3 of a second delay, NCBI limits requests to 3 / second without API key
    $rfh = fopen($fetch_url, "r");
    if (!$rfh) {
      \Drupal::messenger()->addMessage('ERROR: Could not perform PubMed query.', 'error');
      \Drupal::service('tripal.logger')->error("Could not perform PubMed query: $fetch_url.");
      return '';
    }
    $results = '';
    if ($rfh) {
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
   * https://www.nlm.nih.gov/bsd/licensee/elements_descriptions.html
   *
   * Information about PubMed's citation format can be found here
   * https://www.nlm.nih.gov/bsd/policy/cit_format.html
   *
   * @param $pub_xml
   *  An XML string describing a single publication
   *
   * @return
   *  An array describing the publication
   *
   * @ingroup tripal_pub
   */
  public function parse($pub_xml) {
    $pub = [];
  
    if (!$pub_xml) {
      return $pub;
    }
  
    // read the XML and iterate through it.
    $xml = new \XMLReader();
    $xml->xml(trim($pub_xml));
    while ($xml->read()) {
      $element = $xml->name;
      if ($xml->nodeType == \XMLReader::ELEMENT) {
  
        switch ($element) {
          case 'ERROR':
            $xml->read(); // get the value for this element
            \Drupal::service('tripal.logger')->error("Error: " . $xml->value);
            break;
          case 'PMID':
            // thre are multiple places where a PMID is present in the XML and
            // since this code does not descend into every branch of the XML tree
            // we will encounter many of them here.  Therefore, we only want the
            // PMID that we first encounter. If we already have the PMID we will
            // just skip it.  Examples of other PMIDs are in the articles that
            // cite this one.
            $xml->read(); // get the value for this element
            if (!array_key_exists('Publication Dbxref', $pub)) {
              $pub['Publication Dbxref'] = 'PMID:' . $xml->value;
            }
            break;
          case 'Article':
            $pub_model = $xml->getAttribute('PubModel');
            $pub['Publication Model'] = $pub_model;
            $this->pmidParseArticle($xml, $pub);
            break;
          case 'MedlineJournalInfo':
            $this->pmidParseMedlineJournalInfo($xml, $pub);
            break;
          case 'BookDocument':
            $this->pmidParseBookDocument($xml, $pub);
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
    $pub['Citation'] = $this->pmid_generate_citation($pub);
  
    // $pub['raw'] = $pub_xml;
    return $pub;
  }

  /**
   * Creates Citation
   * 
   * This function generates citations for publications.  It requires
   * an array structure with keys being the terms in the Tripal
   * publication ontology.  This function is intended to be used
   * for any function that needs to generate a citation.
   *
   * @param $pub
   *   An array structure containing publication details where the keys
   *   are the publication ontology term names and values are the
   *   corresponding details.  The pub array can contain the following
   *   keys with corresponding values:
   *     - Publication Type:  an array of publication types. a publication can
   *       have more than one type.
   *     - Authors: a  string containing all of the authors of a publication.
   *     - Journal Name:  a string containing the journal name.
   *     - Journal Abbreviation: a string containing the journal name
   *   abbreviation.
   *     - Series Name: a string containing the series (e.g. conference
   *       proceedings) name.
   *     - Series Abbreviation: a string containing the series name abbreviation
   *     - Volume: the serives volume number.
   *     - Issue: the series issue number.
   *     - Pages: the page numbers for the publication.
   *     - Publication Date:  A date in the format "Year Month Day".
   *
   * @return
   *   A text string containing the citation.
   */
  private function pmid_generate_citation(&$pub) {
    $citation = '';
    $pub_type = '';
  
    // An article may have more than one publication type. For example,
    // a publication type can be 'Journal Article' but also a 'Clinical Trial'.
    // Therefore, we need to select the type that makes most sense for
    // construction of the citation. Here we'll iterate through them all
    // and select the one that matches best.
    if (is_array($pub['Publication Type'])) {
      foreach ($pub['Publication Type'] as $ptype) {
        if ($ptype == 'Journal Article') {
          $pub_type = $ptype;
          break;
        }
        else {
          if ($ptype == 'Conference Proceedings') {
            $pub_type = $ptype;
            break;
          }
          else {
            if ($ptype == 'Review') {
              $pub_type = $ptype;
              break;
            }
            else {
              if ($ptype == 'Book') {
                $pub_type = $ptype;
                break;
              }
              else {
                if ($ptype == 'Letter') {
                  $pub_type = $ptype;
                  break;
                }
                else {
                  if ($ptype == 'Book Chapter') {
                    $pub_type = $ptype;
                    break;
                  }
                  else {
                    if ($ptype == "Research Support, Non-U.S. Gov't") {
                      $pub_type = $ptype;
                      // We don't break because if the article is also a Journal Article
                      // we prefer that type.
                    }
                  }
                }
              }
            }
          }
        }
      }
      // If we don't have a recognized publication type, then just use the
      // first one in the list.
      if (!$pub_type) {
        $pub_type = $pub['Publication Type'][0];
      }
    }
    else {
      $pub_type = $pub['Publication Type'];
    }
    //----------------------
    // Journal Article
    //----------------------
    if ($pub_type == 'Journal Article') {
      if (array_key_exists('Authors', $pub)) {
        $citation = $pub['Authors'] . '. ';
      }
  
      $citation .= $pub['Title'] . '. ';
  
      if (array_key_exists('Journal Name', $pub)) {
        $citation .= $pub['Journal Name'] . '. ';
      }
      elseif (array_key_exists('Journal Abbreviation', $pub)) {
        $citation .= $pub['Journal Abbreviation'] . '. ';
      }
      elseif (array_key_exists('Series Name', $pub)) {
        $citation .= $pub['Series Name'] . '. ';
      }
      elseif (array_key_exists('Series Abbreviation', $pub)) {
        $citation .= $pub['Series Abbreviation'] . '. ';
      }
      if (array_key_exists('Publication Date', $pub)) {
        $citation .= $pub['Publication Date'];
      }
      elseif (array_key_exists('Year', $pub)) {
        $citation .= $pub['Year'];
      }
      if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
        $citation .= '; ';
      }
      if (array_key_exists('Volume', $pub)) {
        $citation .= $pub['Volume'];
      }
      if (array_key_exists('Issue', $pub)) {
        $citation .= '(' . $pub['Issue'] . ')';
      }
      if (array_key_exists('Pages', $pub)) {
        if (array_key_exists('Volume', $pub)) {
          $citation .= ':';
        }
        $citation .= $pub['Pages'];
      }
      $citation .= '.';
    }
    //----------------------
    // Review
    //----------------------
    else {
      if ($pub_type == 'Review') {
        if (array_key_exists('Authors', $pub)) {
          $citation = $pub['Authors'] . '. ';
        }
  
        $citation .= $pub['Title'] . '. ';
  
        if (array_key_exists('Journal Name', $pub)) {
          $citation .= $pub['Journal Name'] . '. ';
        }
        elseif (array_key_exists('Journal Abbreviation', $pub)) {
          $citation .= $pub['Journal Abbreviation'] . '. ';
        }
        elseif (array_key_exists('Series Name', $pub)) {
          $citation .= $pub['Series Name'] . '. ';
        }
        elseif (array_key_exists('Series Abbreviation', $pub)) {
          $citation .= $pub['Series Abbreviation'] . '. ';
        }
        elseif (array_key_exists('Publisher', $pub)) {
          $citation .= $pub['Publisher'] . '. ';
        }
        if (array_key_exists('Publication Date', $pub)) {
          $citation .= $pub['Publication Date'];
        }
        elseif (array_key_exists('Year', $pub)) {
          $citation .= $pub['Year'];
        }
        if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
          $citation .= '; ';
        }
        if (array_key_exists('Volume', $pub)) {
          $citation .= $pub['Volume'];
        }
        if (array_key_exists('Issue', $pub)) {
          $citation .= '(' . $pub['Issue'] . ')';
        }
        if (array_key_exists('Pages', $pub)) {
          if (array_key_exists('Volume', $pub)) {
            $citation .= ':';
          }
          $citation .= $pub['Pages'];
        }
        $citation .= '.';
      }
      //----------------------
      // Research Support, Non-U.S. Gov't
      //----------------------
      elseif ($pub_type == "Research Support, Non-U.S. Gov't") {
        if (array_key_exists('Authors', $pub)) {
          $citation = $pub['Authors'] . '. ';
        }
  
        $citation .= $pub['Title'] . '. ';
  
        if (array_key_exists('Journal Name', $pub)) {
          $citation .= $pub['Journal Name'] . '. ';
        }
        if (array_key_exists('Publication Date', $pub)) {
          $citation .= $pub['Publication Date'];
        }
        elseif (array_key_exists('Year', $pub)) {
          $citation .= $pub['Year'];
        }
        $citation .= '.';
      }
      //----------------------
      // Letter
      //----------------------
      elseif ($pub_type == 'Letter') {
        if (array_key_exists('Authors', $pub)) {
          $citation = $pub['Authors'] . '. ';
        }
  
        $citation .= $pub['Title'] . '. ';
        if (array_key_exists('Journal Name', $pub)) {
          $citation .= $pub['Journal Name'] . '. ';
        }
        elseif (array_key_exists('Journal Abbreviation', $pub)) {
          $citation .= $pub['Journal Abbreviation'] . '. ';
        }
        elseif (array_key_exists('Series Name', $pub)) {
          $citation .= $pub['Series Name'] . '. ';
        }
        elseif (array_key_exists('Series Abbreviation', $pub)) {
          $citation .= $pub['Series Abbreviation'] . '. ';
        }
        if (array_key_exists('Publication Date', $pub)) {
          $citation .= $pub['Publication Date'];
        }
        elseif (array_key_exists('Year', $pub)) {
          $citation .= $pub['Year'];
        }
        if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
          $citation .= '; ';
        }
        if (array_key_exists('Volume', $pub)) {
          $citation .= $pub['Volume'];
        }
        if (array_key_exists('Issue', $pub)) {
          $citation .= '(' . $pub['Issue'] . ')';
        }
        if (array_key_exists('Pages', $pub)) {
          if (array_key_exists('Volume', $pub)) {
            $citation .= ':';
          }
          $citation .= $pub['Pages'];
        }
        $citation .= '.';
      }
      //-----------------------
      // Conference Proceedings
      //-----------------------
      elseif ($pub_type == 'Conference Proceedings') {
        if (array_key_exists('Authors', $pub)) {
          $citation = $pub['Authors'] . '. ';
        }
  
        $citation .= $pub['Title'] . '. ';
        if (array_key_exists('Conference Name', $pub)) {
          $citation .= $pub['Conference Name'] . '. ';
        }
        elseif (array_key_exists('Series Name', $pub)) {
          $citation .= $pub['Series Name'] . '. ';
        }
        elseif (array_key_exists('Series Abbreviation', $pub)) {
          $citation .= $pub['Series Abbreviation'] . '. ';
        }
        if (array_key_exists('Publication Date', $pub)) {
          $citation .= $pub['Publication Date'];
        }
        elseif (array_key_exists('Year', $pub)) {
          $citation .= $pub['Year'];
        }
        if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
          $citation .= '; ';
        }
        if (array_key_exists('Volume', $pub)) {
          $citation .= $pub['Volume'];
        }
        if (array_key_exists('Issue', $pub)) {
          $citation .= '(' . $pub['Issue'] . ')';
        }
        if (array_key_exists('Pages', $pub)) {
          if (array_key_exists('Volume', $pub)) {
            $citation .= ':';
          }
          $citation .= $pub['Pages'];
        }
        $citation .= '.';
      }
      //-----------------------
      // Default
      //-----------------------
      else {
        if (array_key_exists('Authors', $pub)) {
          $citation = $pub['Authors'] . '. ';
        }
        $citation .= $pub['Title'] . '. ';
        if (array_key_exists('Series Name', $pub)) {
          $citation .= $pub['Series Name'] . '. ';
        }
        elseif (array_key_exists('Series Abbreviation', $pub)) {
          $citation .= $pub['Series Abbreviation'] . '. ';
        }
        if (array_key_exists('Publication Date', $pub)) {
          $citation .= $pub['Publication Date'];
        }
        elseif (array_key_exists('Year', $pub)) {
          $citation .= $pub['Year'];
        }
        if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
          $citation .= '; ';
        }
        if (array_key_exists('Volume', $pub)) {
          $citation .= $pub['Volume'];
        }
        if (array_key_exists('Issue', $pub)) {
          $citation .= '(' . $pub['Issue'] . ')';
        }
        if (array_key_exists('Pages', $pub)) {
          if (array_key_exists('Volume', $pub)) {
            $citation .= ':';
          }
          $citation .= $pub['Pages'];
        }
        $citation .= '.';
      }
    }
  
    return $citation;
  }

  /**
   * Parses the section from the XML returned from PubMed that contains
   * information about a book.
   *
   * @param $xml
   *   The XML to parse
   * @param $pub
   *   The publication object to which additional details will be added
   *
   * @ingroup tripal_pub
   */
  private function pmidParseBookDocument($xml, &$pub) {

    while ($xml->read()) {
      // get this element name
      $element = $xml->name;

      // if we're at the </Book> element then we're done with the book...
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'BookDocument') {
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
        switch ($element) {
          case 'PMID':
            $xml->read(); // get the value for this element
            if (!array_key_exists('Publication Dbxref', $pub)) {
              $pub['Publication Dbxref'] = 'PMID:' . $xml->value;
            }
            break;
          case 'BookTitle':
            $pub['Title'] = $xml->readString();
            break;
          case 'ArticleTitle':
            // This can happen if there is a chapter in a book, append to the book title
            $title = $xml->readString();
            $pub['Title'] = $pub['Title'] ? ($pub['Title'] . '. ' . $title) : $title;
            break;
          case 'Abstract':
            $this->pmidParseAbstract($xml, $pub);
            break;
          case 'Pagination':
            $this->pmidParsePagination($xml, $pub);
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
            $this->pmidParseAuthorlist($xml, $pub);
            break;
          case 'Language':
            $xml->read();
            $lang_abbr = $xml->value;
            // there may be multiple languages so we store these in an array
            $pub['Language'][] = $this->remoteSearchGetLanguage($lang_abbr);
            $pub['Language Abbr'][] = $lang_abbr;
            break;
          case 'PublicationTypeList':
            $this->pmidParsePublicationTypeList($xml, $pub);
            break;
          case 'PublicationType':
            $this->pmidParsePublicationType($xml, $pub);
            break;
          case 'VernacularTitle':
            $xml->read();
            $pub['Vernacular Title'][] = $xml->value;
            break;
          case 'PublisherName':
            $xml->read();
            $pub['Publisher'] = $xml->value;
            break;
          case 'PubDate':
            $date = $this->pmidParseDate($xml, 'PubDate');
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
   * a list of publication types
   *
   * @param $xml
   *   The XML to parse
   * @param $pub
   *   The publication object to which additional details will be added
   *
   * @ingroup tripal_pub
   */
  private function pmidParsePublicationTypeList($xml, &$pub) {

    while ($xml->read()) {
      $element = $xml->name;

      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'PublicationTypeList') {
        // we've reached the </PublicationTypeList> element so we're done.
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
        switch ($element) {
          case 'PublicationType':
            $this->pmidParsePublicationType($xml, $pub);
            break;
          default:
            break;
        }
      }
    }
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
  private function pmidParseMedlineJournalInfo($xml, &$pub) {
    while ($xml->read()) {
      // get this element name
      $element = $xml->name;
  
      // if we're at the </Article> element then we're done with the article...
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'MedlineJournalInfo') {
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
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
  private function pmidParseArticle($xml, &$pub) {
  
    while ($xml->read()) {
      // get this element name
      $element = $xml->name;
  
      // if we're at the </Article> element then we're done with the article...
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'Article') {
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
        switch ($element) {
          case 'Journal':
            $this->pmidParseJournal($xml, $pub);
            break;
          case 'ArticleTitle':
            $pub['Title'] = $xml->readString();
            break;
          case 'Abstract':
            $this->pmidParseAbstract($xml, $pub);
            break;
          case 'Pagination':
            $this->pmidParsePagination($xml, $pub);
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
            $this->pmidParseAuthorlist($xml, $pub);
            break;
          case 'InvestigatorList':
            // TODO: perhaps handle this one day.  The investigator list is to list the names of people who
            // are members of a collective or corporate group that is an author in the paper.
            break;
          case 'Language':
            $xml->read();
            $lang_abbr = $xml->value;
            // there may be multiple languages so we store these in an array
            $pub['Language'][] = $this->remoteSearchGetLanguage($lang_abbr);
            $pub['Language Abbr'][] = $lang_abbr;
            break;
          case 'DataBankList':
            // TODO: handle this case
            break;
          case 'GrantList':
            // TODO: handle this case
            break;
          case 'PublicationTypeList':
            $this->pmidParsePublicationTypeList($xml, $pub);
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
  private function pmidParsePublicationType($xml, &$pub) {
    $chado = \Drupal::service('tripal_chado.database');
    $xml->read();
    $value = $xml->value;

    $identifiers = [
      'name' => $value,
      'cv_id' => [
        'name' => 'tripal_pub',
      ],
    ];
    $options = ['case_insensitive_columns' => ['name']];
    $pub_cvterm = chado_get_cvterm($identifiers, $options, $chado->getSchemaName());
    if (!$pub_cvterm) {
      // see if this we can find the name using a synonym
      $identifiers = [
        'synonym' => [
          'name' => $value,
          'cv_name' => 'tripal_pub',
        ],
      ];
      $pub_cvterm = chado_get_cvterm($identifiers, $options, $chado->getSchemaName());
      if (!$pub_cvterm) {
        \Drupal::service('tripal.logger')->error('Cannot find a valid vocabulary term for the publication type: "' . 
          $value . '"');
      }
    }
    else {
      $pub['Publication Type'][] = $pub_cvterm->name;
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
  private function pmidParseAbstract($xml, &$pub) {
    $abstract = '';
  
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'Abstract') {
        // we've reached the </Abstract> element so return
        $pub['Abstract'] = $abstract;
        return;
      }
      // the abstract text can be just a single paragraph or be broken into multiple
      // abstract texts for structured abstracts.  Here we will just combine then
      // into a single element in the order that they arrive in HTML format
      if ($xml->nodeType == \XMLReader::ELEMENT) {
        switch ($element) {
          case 'AbstractText':
            $label = $xml->getAttribute('Label');
            $value = $xml->readString();
            if ($label) {
              $part = "<p><b>$label</b></br>" . $value . '</p>';
              $abstract .= $part;
              $pub['Structured Abstract Part'][] = $part;
            }
            else {
              $abstract .= "<p>" . $value . "</p>";
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
  private function pmidParsePagination($xml, &$pub) {
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'Pagination') {
        // we've reached the </Pagination> element so we're done.
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
        switch ($element) {
          case 'MedlinePgn':
            $xml->read();
            if (trim($xml->value)) {
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
  private function pmidParseJournal($xml, &$pub) {
  
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'Journal') {
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
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
            $this->pmidParseJournalIssue($xml, $pub);
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
  private function pmidParseJournalIssue($xml, &$pub) {
  
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == 'JournalIssue') {
        // if we're at the </JournalIssue> element then we're done
        return;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
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
            $date = $this->pmidParseDate($xml, 'PubDate');
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
  private function pmidParseDate($xml, $element_name) {
    $date = [];
  
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT and $element == $element_name) {
        // if we're at the </$element_name> then we're done
        return $date;
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
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
            if (!$date['year']) {
              $date['year'] = preg_replace('/^.*(\d{4}).*$/', '\1', $xml->value);
            }
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
  private function pmidParseAuthorlist($xml, &$pub) {
    $num_authors = 0;
  
    while ($xml->read()) {
      $element = $xml->name;
  
      if ($xml->nodeType == \XMLReader::END_ELEMENT) {
        // if we're at the </AuthorList> element then we're done with the article...
        if ($element == 'AuthorList') {
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
              $authors .= ($author['Surname']??'') . ' ' . ($author['First Initials']??'') . ', ';
            }
          }
          $authors = substr($authors, 0, -2);
          $pub['Authors'] = $authors;
          return;
        }
        // if we're at the end </Author> element then we're done with the author and we can
        // start a new one.
        if ($element == 'Author') {
          $num_authors++;
        }
      }
      if ($xml->nodeType == \XMLReader::ELEMENT) {
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
  private function remoteSearchGetLanguage($lang_abbr) {
    $languages = [
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
    ];
    return $languages[strtolower($lang_abbr)];
  }

}