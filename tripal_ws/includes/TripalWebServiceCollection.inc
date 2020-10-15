<?php

class TripalWebServiceCollection extends TripalWebServiceResource {

  /**
   * Holds the data portion of the JSON-LD response if this resource is
   * a collection.
   */
  protected $members;

  /**
   * Set to TRUE if paging should be enabled.  Paging is disabled by default.
   */
  protected $doPaging;

  /**
   * The total number of items.  This variable is used if paging is turned on.
   * Otherwise it's ignored and the total number of items reported by the
   * collection will be the number of elements in the $members array.
   */
  protected $totalItems;

  /**
   * When the collection contains more than the itemsPerpage amount then
   * the results will be paged.
   */
  protected $itemsPerPage;

  /**
   * The current page of the pager.
   */
  protected $page;


  /**
   * The parameters as collected by the parent TripalWebService.  We need
   * the parameters because this Resource generates first, next, prev, and last
   * links and we need to maintain the parameters a user may have
   * provided in those links.
   */
  protected $params;

  /**
   * Implements the constructor.
   *
   * @param TripalWebService $service
   *   An instance of a TripalWebService or class that extends it.
   */
  public function __construct($service_path, $params) {
    parent::__construct($service_path);

    // Initialize private member variables.
    $this->params = $params;
    $this->members = [];

    // Add some terms to the context.
    $this->addContextItem('Collection', 'hydra:Collection');
    $this->addContextItem('totalItems', 'hydra:totalItems');
    $this->addContextItem('member', 'hydra:member');
    parent::setType('Collection');

    // If the totalItems is set to -1 then this means paging is turned off and
    // all of the elements in the $memgbers array should be used.
    $this->totalItems = 0;
    $this->itemsPerPage = 25;
    $this->doPaging = FALSE;
  }

  /**
   * Initializes the pager.
   *
   * @param $totalItems
   *   The total number of items available.
   * @param $itemsPerPage
   *   The maximum number of items per page.
   * @param $path
   *   The path
   */
  public function initPager($totalItems, $itemsPerPage, $page) {
    $this->doPaging = TRUE;
    $this->totalItems = $totalItems;
    $this->itemsPerPage = $itemsPerPage;
    $this->page = $page;
  }

  /**
   * Adds a new member to this resource if it is a collection.
   *
   * @param $member
   *   A TripalWebServiceResource member whose type is the same as this
   *   resource
   */
  public function addMember($member) {
    // Make sure the $service provides is a TripalWebService class.
    if (!is_a($member, 'TripalWebServiceResource')) {
      throw new Exception("Cannot add a new member to this resource collection as it is not a TripalWebServiceResource.");
    }
    $this->members[] = $member;
  }

  /**
   * @see TripalWebServiceResource::setType()
   */
  public function setType($type) {
    // TODO: There should be a check to make sure that the type is a
    // subclass of the hydra:Collection term.
    parent::setType($type);
  }

  /**
   * Retrieves the data section of the resource.
   *
   * The JSON-LD response constists of two sections the '@context' section
   * and the data section.  This function only returns the data section
   * for this resource
   *
   * @return
   *   An associative array containing the data section of the response.
   */
  public function getData() {
    $data = $this->data;
    $data['totalItems'] = 0;

    if ($this->doPaging == TRUE) {

      // Save any parameters provided by the user
      $saved_params = '';
      foreach ($this->params as $pkey => $pval) {
        if (in_array($pkey, [
          'page',
          'limit',
          'first',
          'last',
          'next',
          'prev',
        ])) {
          continue;
        }
        $saved_params .= '&' . $pkey . '=' . $pval;
      }

      $data['totalItems'] = $this->totalItems;
      $total_pages = ceil($this->totalItems / $this->itemsPerPage);
      $page = $this->page;
      $limit = $this->itemsPerPage;

      if ($this->totalItems > 0) {
        $data['view'] = [
          '@id' => $this->service_path . '?' . implode('&', array_merge([
              "page=$page",
              "limit=$limit",
            ])) . $saved_params,
          '@type' => 'PartialCollectionView',
          'first' => $this->service_path . '?' . implode('&', array_merge([
              "page=1",
              "limit=$limit",
            ])) . $saved_params,
          'last' => $this->service_path . '?' . implode('&', array_merge([
              "page=$total_pages",
              "limit=$limit",
            ])) . $saved_params,
        ];
        $prev = $page - 1;
        $next = $page + 1;
        if ($prev > 0) {
          $data['view']['previous'] = $this->service_path . '?' . implode('&', [
              "page=$prev",
              "limit=$limit",
            ]) . $saved_params;
        }
        if ($next < $total_pages) {
          $data['view']['next'] = $this->service_path . '?' . implode('&', [
              "page=$next",
              "limit=$limit",
            ]) . $saved_params;
        }
      }
    }
    else {
      $data['totalItems'] = count($this->members);
    }

    $member_data = [];
    foreach ($this->members as $key => $member) {
      $member_data[] = $member->getData();
    }
    $data['member'] = $member_data;

    // If paging of this collection is enabled then add the pager control links.

    return $data;
  }

  /**
   * Retrieves the data section of the resource.
   *
   * The JSON-LD response constists of two sections the '@context' section
   * and the data section.  This function only returns the data section
   * for this resource
   *
   * @return
   *   An associative array containing the data section of the response.
   */
  public function getContext() {
    if ($this->doPaging == TRUE) {
      $this->addContextItem('view', 'hydra:PartialCollectionView');
    }
    $context = $this->context;
    foreach ($this->members as $key => $member) {
      $citems = $member->getContext();
      foreach ($citems as $key => $val) {
        $context[$key] = $val;
      }
    }
    return $context;
  }
}

