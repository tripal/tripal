<?php

namespace Drupal\tripal\TripalPubLibrary;
use Drupal\tripal\TripalPubLibrary\Interfaces\TripalPubLibraryInterface;
use Drupal\Component\Plugin\PluginBase;
use Drupal\file\Entity\File;
use Drupal\user\Entity\User;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;

/**
 * Defines the base class for the tripal pub parser plugins.
 */
abstract class TripalPubLibraryBase extends PluginBase implements TripalPubLibraryInterface {


  /**
   * The ID of this plugin.
   *
   * @var string
   */
  protected $plugin_id;

  /**
   * The plugin definition
   *
   * @var array
   */
  protected $plugin_definition;


  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
  }

}
