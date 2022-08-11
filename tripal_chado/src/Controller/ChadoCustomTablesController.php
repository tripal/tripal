<?php

namespace Drupal\tripal_chado\Controller;

use Drupal\Core\Breadcrumb\Breadcrumb;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Controller routines for the Tripal Module
 */
class ChadoCustomTablesController extends ControllerBase {

    /**
     * Constructs the TripalJobController.
     *
     */
    public function __construct() {

    }

  /**
   * Provides the main landing page for managing Jobs.
   */
  public function admin_custom_tables() {

    // set the breadcrumb
    $breadcrumb = new Breadcrumb();
    $breadcrumb->addLink(Link::fromTextAndUrl('Home',
        Url::fromRoute('<front>')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Administration',
        Url::fromUri('internal:/admin')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Tripal',
        Url::fromUri('internal:/admin/tripal')));



    $view = \Drupal\views\Views::getView('chado_custom_tables');
    $view->setDisplay('default');
    if ($view->access('default')) {
      return $view->render();
    }
    else {
      return [
        '#markup' => 'You do not have access to view this page.',
      ];
    }
  }

}