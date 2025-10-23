<?php

namespace Drupal\tripal\Plugin\views\access;

use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\Routing\Route;
use Drupal\tripal\Access\TripalEntityAccessControlHandler;
use Drupal\views\Plugin\views\access\AccessPluginBase;
use Drupal\Core\Session\AccountInterface;
use Drupal\views\Attribute\ViewsAccess;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Allows Views to reuse the CRUD permissions for Tripal Content.
 *
 * Note: This also allows them to respect the 'administer tripal content'
 * override.
 */
#[ViewsAccess(
  id: new TranslatableMarkup("tripal_content_views_access"),
  title: new TranslatableMarkup("Use Tripal Content permissions"),
  help: new TranslatableMarkup("Applies the same permissions used for viewing Tripal Content pages based on the types configured."),
)]
class TripalContentViewAccessHandler extends AccessPluginBase {

  use StringTranslationTrait;

  /**
   * {@inheritdoc}
   */
  protected $usesOptions = TRUE;

  /**
   * {@inheritdoc}
   */
  protected function defineOptions() {
    $options = parent::defineOptions();

    // Which content types the permissions should be restricted to.
    $options['content_types'] = ['default' => ['all']];
    // The way to handle multiple content types.
    $options['mode'] = ['default' => 'any'];
    // The CRUD operation permission for those content types to check.
    $options['operation'] = ['default' => 'view'];

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);

    // @todo generate this list programatically using the TripalContentType id
    // for the key and the label for the value.
    $content_type_options = [
      'organism' => $this->t('Organism'),
      'project' => $this->t('Project'),
    ];
    // Add an options for "All" content types to support where the
    // content types change.
    $content_type_options['all'] = $this->t('ALL Existing Content Types');

    $form['content_types'] = [
      '#type' => 'select',
      '#title' => $this->t('Content Types'),
      '#default_value' => $this->options['content_types'],
      '#options' => $content_type_options,
      '#description' => $this->t('Select which content types whose Tripal Content permissions you want to apply to this view.'),
      '#multiple' => TRUE,
      '#required' => TRUE,
    ];

    $form['mode'] = [
      '#type' => 'radios',
      '#title' => $this->t('Multiple Permission Handling'),
      '#default_value' => $this->options['mode'],
      '#description' => $this->t('Indicate how you want multiple content type permissions to be handled.'),
      '#required' => TRUE,
      '#options' => [
        'any' => $this->t('ANY: user has permission to at least one of the content types selected.'),
        'all' => $this->t('ALL: user must have access to all the content types selected.'),
      ],
    ];

    $form['operation'] = [
      '#type' => 'radios',
      '#title' => $this->t('Content Type Permission'),
      '#default_value' => $this->options['operation'],
      '#description' => $this->t('Indicate which of the permissions to use for each content type.'),
      '#required' => TRUE,
      '#options' => [
        'view' => $this->t('VIEW: user has permission to view the selected content type(s).'),
        'create' => $this->t('CREATE: user has permission to view the selected content type(s).'),
        'edit' => $this->t('EDIT: user has permission to edit the selected content type(s).'),
        'unpublish' => $this->t('UNPUBLISH: user has permission to view the selected content type(s).'),
        'delete' => $this->t('DELETE: user has permission to delete the selected content type(s).'),
      ],
    ];

  }

  /**
   * {@inheritdoc}
   *
   * Note: This is called when the view is being rendered. This allows us to
   * catch even programatic rendering or embedding at separate routes.
   */
  public function access(AccountInterface $account) {

    // Let's just use the TripalEntity Access controller directly here.
    return TripalEntityAccessControlHandler::checkManyPermissions(
      $account,
      $this->options['content_types'],
      $this->options['operation'],
      $this->options['mode']
    )->isAllowed();
  }

  /**
   * {@inheritdoc}
   *
   * Note: Altering the route allows Drupal to hide links, etc. before the
   * page is accessed or rendered.
   */
  public function alterRouteDefinition(Route $route) {

    // Add a requirement that points to the correct static method in the
    // TripalEntity access controller.
    $route->setRequirement('_custom_access', static::class . '::checkManyPermissions');
    // Sets the options that this method expects.
    $route->setOption('_content_types', $this->options['content_types']);
    $route->setOption('_comparison_mode', $this->options['mode']);
    $route->setOption('_permission_operation', $this->options['operation']);
  }

  /**
   * Grabs paramters from route for use with the TripalEntity Access Controller.
   *
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The user to check permissions for.
   * @param \Symfony\Component\Routing\Route $route
   *   The calling route which gives access to the options.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The result of this permission check.
   */
  public static function checkManyPermissions(AccountInterface $account, Route $route): AccessResultInterface {

    // Get the options from the route that we set before.
    // @see alterRouteDefinition().
    $content_types = $route->getOption('_content_types');
    $mode = $route->getOption('_comparison_mode');
    $operation = $route->getOption('_permission_operation');

    // Now let's use the TripalEntity Access controller directly here.
    return TripalEntityAccessControlHandler::checkManyPermissions(
      $account,
      $content_types,
      $operation,
      $mode
    );
  }

  /**
   * {@inheritdoc}
   */
  public function summaryTitle() {

    $num_content_types = count($this->options['content_types']);
    $operation = $this->options['operation'];

    // Expand mode to fit in the statement.
    $mode_string = 'at least 1 of';
    if ($this->options['mode'] === 'all') {
      $mode_string = 'all of';
    }

    // If the 'all' content type options was chosen then change the statement.
    if (array_key_exists('all', $this->options['content_types'])) {
      $num_content_types = 'the existing';
    }

    // Return the title displayed in the Views UI.
    // @codingStandardsIgnoreStart
    return $this->t("$operation $mode_string $num_content_types Tripal Content Type(s)");
    // @codingStandardsIgnoreEnd
  }

}
