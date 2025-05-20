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
 *    id = "tripal_pub_library_PMID",
 *    label = @Translation("NIH PubMed database"),
 *    description = @Translation("Retrieves and parses publication data from the NIH PubMed database"),
 *  )
 *  n.b. last part of id must match the record in the chado.db table name column
 */
class TripalPubLibraryPubMed extends TripalPubLibraryBase {

  /**
   * Stores information of an initialized search.
   * Array keys are 'Count', 'WebEnv', and 'QueryKey'
   * with values as returned by PubMed's esearch utility
   *
   * @var array $webquery
   */
  protected array $webquery = [];

  public function formSubmit(array $form, \Drupal\Core\Form\FormStateInterface &$form_state): void {
    // DUMMY function from inheritance so it had to be kept.
    // The form_submit function which is called by TripalPubLibrary
    // is needed to receive and process the criteria data. See below.
  }

  /**
   * Plugin specific form submit to add form values to the criteria array.
   * The criteria array eventually gets serialized and stored in the tripal_pub_import
   * database table. (This code gets called from ChadoNewPublicationForm)
   */
  public function form_submit(array $form, \Drupal\Core\Form\FormStateInterface $form_state, array &$criteria): void {
    $user_input = $form_state->getUserInput();
    $criteria['days'] = $user_input['days'];
    $criteria['ncbi_api_key'] = $user_input['ncbi_api_key'];

    // If an NCBI API key was entered, store it as the default for new queries
    if ($criteria['ncbi_api_key']) {
      \Drupal::state()->set('tripal_pub_importer_ncbi_api_key', $criteria['ncbi_api_key']);
    }
  }

  /**
   * Adds plugin specific form items and returns the $form array
   */
  public function form(array $form, \Drupal\Core\Form\FormStateInterface &$form_state): array {
    $default_api_key = \Drupal::state()->get('tripal_pub_importer_ncbi_api_key', '');

    // Add form elements specific to this parser.
    $api_key_description = t('Tripal imports publications using NCBI\'s ')
      . Link::fromTextAndUrl('EUtils API',
          Url::fromUri('https://www.ncbi.nlm.nih.gov/books/NBK25500/', [
            'attributes' => [
              'target' => 'blank',
            ]]))->toString()
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
      '#default_value' => $default_api_key,
      '#size' => 20,
    ];

    $form['pub_library']['days'] = [
      '#title' => t('Days since record modified'),
      '#type' => 'textfield',
      '#description' => t('Limit the search to include pubs that have been added no more than this many days before today.'),
      '#required' => FALSE,
      '#default_value' => 30,
      '#size' => 5,
    ];
    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate(array $form, \Drupal\Core\Form\FormStateInterface &$form_state): void {
    // Perform any form validations necessary with the form data
    $form_state_values = $form_state->getValues();
    $days = $form_state_values['days'] ?? '';
    if (preg_match('/\D/', $days)) {
      $form_state->setErrorByName('days', t('"Days since record modified" must be a non-negative integer, or blank'));
    }
  }


  /**
   * Retrieves one or more publications from PubMed based on a search query specification
   *
   * @param array $query
   *   An associative array defining a publication query,
   *   specifying the database and query parameters for
   *   a particular publication repository.
   *
   * @return array|NULL
   *   - 'total_records' = The number of records available for retrieval
   *   - 'skipped_records' = The number of records where download failed
   *   - 'search_str' = The query string used for the search
   *   - 'pubs' = The uniform publication information array.
   *   or NULL if query failed and an exception was caught
   */
  public function run(array $query): ?array {
    $page = $query['page'];
    $num_to_retrieve = $query['count'];

    $page_results = $this->retrieve($query, $num_to_retrieve, $page);
    return $page_results;
  }

  /**
   * More documentation can be found in TripalPubLibraryInterface
   */
  public function retrieve(array $query, int $limit = 10, int $page = 0): ?array {
    $results = NULL;
    try {
      $results = $this->remoteSearchPMID($query, $limit, $page);
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
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
   * @return array|NULL
   *   - 'total_records' = The number of records available for retrieval
   *   - 'skipped_records' = The number of records where download failed
   *   - 'search_str' = The query string used for the search
   *   - 'pubs' = The uniform publication information array.
   *   or NULL if query failed and an exception was caught
   *
   * @ingroup tripal_pub
   */
  public function remoteSearchPMID($search_array, $num_to_retrieve, $page, $row_mode = 1): ?array {
    $api_key = $search_array['ncbi_api_key'] ?? $search_array['form_state_user_input']['ncbi_api_key'] ?? '';
    // Only initialize for page zero, subsequent pages use the established query
    if ($page == 0) {
      $days = $search_array['days'] ?? '';

      // convert the terms list provided by the caller into a string with words
      // separated by a '+' symbol.
      $num_criteria = $search_array['num_criteria'];
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

      // Initialize the remote query
      if (!$this->pmidSearchInit($search_str, $num_to_retrieve, $api_key)) {
        return NULL;
      }
    }

    // initialize the retrieval loop
    $total_records = $this->webquery['Count'];
    $start = $page * $num_to_retrieve;

    // if we have no records then return an empty array
    if (($total_records == 0) or ($start > $total_records)) {
      return [
        'total_records' => $total_records,
        'skipped_records' => 0,
        'search_str' => '',
        'pubs' => [],
      ];
    }

    // Get the list of PMIDs from the initialized search
    $pmids_txt = $this->pmidFetch('uilist', 'text', $start, $num_to_retrieve, $api_key, []);
    if (is_null($pmids_txt)) {
      return NULL;
    }

    // Iterate through each PMID and download and parse its publication record.
    $pmids = explode("\n", trim($pmids_txt));
    $pubs = [];
    $n_skipped = 0;
    foreach ($pmids as $pmid) {
      // Retrieve and parse each record.
      $pub_xml = $this->pmidFetch('null', 'xml', 0, 1, $api_key, ['id' => $pmid]);
      if (is_null($pub_xml)) {
        // Skip over any individual publication that had a download error
        $n_skipped++;
        $this->logger->error('Skipping publication @acc due to download error.',
          ['@acc' => $pmid]);
      }
      else {
        $pub = $this->parse_xml($pub_xml);
        $pubs[] = $pub;
      }
    }

    // Note that search_str is only returned for the first page
    return [
      'total_records' => $total_records,
      'skipped_records' => $n_skipped,
      'search_str' => $search_str ?? '',
      'pubs' => $pubs,
    ];
  }

  /**
   * Initailizes a PubMed Search using a given search string.
   * Values are stored in $this->webquery, which is an array
   * containing the Count, WebEnv and QueryKey as returned
   * by PubMed's esearch utility.
   *
   * @param string $search_str
   *   The PubMed Search string
   * @param int $retmax
   *   The maximum number of records to return
   * @param string $api_key
   *   The optional NCBI API key
   *
   * @return bool
   *   TRUE for success, FALSE for error.
   *
   * @ingroup tripal_pub
   */
  private function pmidSearchInit(string $search_str, int $retmax, string $api_key = ''): bool {

    // do a search for a single result so that we can establish a history, and get
    // the number of records. Once we have the number of records we can retrieve
    // those requested in the range.
    $query_url = "https://www.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?" .
      "db=Pubmed" .
      "&retmax=$retmax" .
      "&usehistory=y" .
      "&term=" . urlencode($search_str);

    $sleep_time = 333334;
    if ($api_key) {
      $query_url .= "&api_key=" . $api_key;
      $sleep_time = 100000;
    }

    usleep($sleep_time);  // 1/3 of a second delay, NCBI limits requests to 3 / second without API key
    $rfh = fopen($query_url, "r");
    if (!$rfh) {
      $this->logger->error("Could not perform Pubmed query. Cannot connect to Entrez.");
      return FALSE;
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
    $this->webquery = [];
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
            $this->webquery['Count'] = $xml->value;
            break;
          case 'WebEnv':
            $xml->read();
            $this->webquery['WebEnv'] = $xml->value;
            break;
          case 'QueryKey':
            $xml->read();
            $this->webquery['QueryKey'] = $xml->value;
            break;
        }
      }
    }
    return TRUE;
  }

  /**
   * Retrieves from PubMed a set of publications from the
   * previously initiated query.
   *
   * @param string $rettype
   *   The efetch return type
   * @param string $retmod
   *   The efetch return mode
   * @param int $start
   *   The start of the range to retrieve
   * @param int $limit
   *   The number of publications to retrieve
   * @param string $api_key
   *   The optional NCBI API key
   * @param array $args
   *   Any additional arguments to add the efetch query URL
   *
   * @return string|NULL
   *   XML as returned from NCBI. Returns NULL if a download error occurred.
   *
   * @ingroup tripal_pub
   */
  private function pmidFetch(string $rettype, string $retmod, int $start,
                             int $limit, string $api_key, array $args): ?string {

    // repeat the search performed previously (using WebEnv & QueryKey) to retrieve
    // the PMID's within the range specied.  The PMIDs will be returned as a text list
    $fetch_url = "https://www.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?" .
      "rettype=$rettype" .
      "&retmode=$retmod" .
      "&retstart=$start" .
      "&retmax=$limit" .
      "&db=Pubmed" .
      "&query_key=" . $this->webquery['QueryKey'] .
      "&WebEnv=" . $this->webquery['WebEnv'];

    $sleep_time = 333334;
    if ($api_key) {
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
    usleep($sleep_time);  // 1/3 or 1/10 of a second delay, NCBI limits requests to 3 / second without API key

    $results = $this->fileretriever->retrieveFileContents($fetch_url);

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
   * @param string $pub_xml
   *   An XML string describing a single publication
   *
   * @return array
   *   An array describing the publication
   *
   * @ingroup tripal_pub
   */
  public function parse_xml(string $pub_xml): array {
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
          case 'Article':
            $pub_model = $xml->getAttribute('PubModel');
            $pub['Publication Model'] = $pub_model;
            $this->pmidParseArticle($xml, $pub);
            break;
          case 'BookDocument':
            $this->pmidParseBookDocument($xml, $pub);
            break;
          // case 'ChemicalList':
            // TODO: handle this
            // break;
          // case 'CitationSubset':
            // TODO: not sure this is needed.
            // break;
          // case 'CommentsCorrections':
            // TODO: handle this
            // break;
          // case 'DeleteCitation':
            // TODO: need to know how to handle this
            // break;
          case 'ERROR':
            $xml->read(); // get the value for this element
            $this->logger->error('XML Internal Error: @err', ['@err' => $xml->value]);
            break;
          // case 'GeneralNote':
            // TODO: handle this
            // break;
          // case 'GeneSymbolList':
            // TODO: handle this
            // break;
          // case 'InvestigatorList':
            // TODO: personal names of individuals who are not authors (can be used with collection)
            // break;
          // case 'KeywordList':
            // TODO: handle this
            // break;
          case 'MedlineJournalInfo':
            $this->pmidParseMedlineJournalInfo($xml, $pub);
            break;
          // case 'MeshHeadingList':
            // TODO: Medical subject headings
            // break;
          // case 'NumberOfReferences':
            // TODO: not sure we should keep this as it changes frequently.
            // break;
          // case 'OtherAbstract':
            // TODO: when the journal does not contain an abstract for the publication.
            // break;
          // case 'OtherID':
            // TODO: ID's from another NLM partner.
            // break;
          // case 'PersonalNameSubjectList':
            // TODO: for works about an individual or with biographical note/obituary.
            // break;
          case 'PMID':
            // There are multiple places where a PMID is present in the XML and
            // since this code does not descend into every branch of the XML tree,
            // we will encounter many of them here. Therefore, we only want the
            // PMID that we first encounter. If we already have the PMID we will
            // just skip it.  Examples of other PMIDs are in the articles that
            // cite this one.
            $xml->read(); // get the value for this element
            if (!array_key_exists('Publication Dbxref', $pub)) {
              $pub['Publication Dbxref'] = $xml->value;
            }
            break;
          // case 'SupplMeshList':
            // TODO: meant for protocol list
            // break;
          default:
            break;
        }
      }
    }

    if ($pub) {
      $pub['Citation'] = $this->pmid_generate_citation($pub);
    }

    return $pub;
  }

  /**
   * Creates a citation for a publication.
   *
   * This function generates a citation for a publication. It requires
   * an array structure with keys being the terms in the Tripal
   * publication ontology.
   *
   * @param $pub
   *   An array structure containing publication details where the keys
   *   are the publication ontology term names and values are the
   *   corresponding details. The pub array can contain the following
   *   keys with corresponding values:
   *     - Publication Type: an array of publication types. a publication can
   *       have more than one type.
   *     - Authors: a string containing all of the authors of a publication.
   *     - Journal Name: a string containing the journal name.
   *     - Journal Abbreviation: a string containing the journal name
   *       abbreviation.
   *     - Series Name: a string containing the series (e.g. conference
   *       proceedings) name.
   *     - Series Abbreviation: a string containing the series name abbreviation
   *     - Volume: the serives volume number.
   *     - Issue: the series issue number.
   *     - Pages: the page numbers for the publication.
   *     - Publication Date: a date in the format "Year Month Day".
   *
   * @return
   *   A text string containing the citation.
   */
  private function pmid_generate_citation($pub) {

    $pub_type = $this->pmid_get_pub_type($pub);

    // The citation manager uses a default citation format if the passed
    // $pub_type is not known.
    $citation_format = $this->citation_manager->getDefaultCitationTemplate($pub_type);
    $citation = $this->citation_manager->generateCitation($citation_format, $pub);

    return $citation;
  }

  /**
   * Determines the type of publication
   *
   * @param $pub
   *   An array structure containing publication details where the keys
   *   are the publication ontology term names and values are the
   *   corresponding details.
   *
   * @return string
   *   The publication type, e.g. "Journal Article", "Book", etc.
   */
  private function pmid_get_pub_type($pub): string {
    $pub_type = '';
    if (array_key_exists('Publication Type', $pub)) {
      $known_types = [
        'Book',
        'Book Chapter',
        'Conference Proceedings',
        'Journal Article',
        'Letter',
        'Review',
      ];

      // An article may have more than one publication type. For example,
      // a publication type can be 'Journal Article' but also a 'Clinical Trial'.
      // Therefore, we need to select the type that makes most sense for
      // construction of the citation. Here we'll iterate through them all
      // and select the one that matches best.
      if (is_array($pub['Publication Type'])) {
        foreach ($pub['Publication Type'] as $ptype) {
          if (in_array($ptype, $known_types)) {
            $pub_type = $ptype;
            break;
          }
          elseif ($ptype == "Research Support, Non-U.S. Gov't") {
            $pub_type = $ptype;
            // We don't break because if the article is also a Journal Article
            // we prefer that type.
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
    }
    return $pub_type;
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
              $pub['Publication Dbxref'] = $xml->value;
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
   * @param XMLReader $xml
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
    $xml->read();
    $value = $xml->value;
    if ($value) {
      $pub['Publication Type'][] = $value;
    }
    return;
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
   * @param XMLReader $xml
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
        // if we're at the end </Author> element then we're done with the author
        // and we can start a new one.
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
