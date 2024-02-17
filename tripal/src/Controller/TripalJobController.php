<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Breadcrumb\Breadcrumb;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Render\Markup;
use Drupal\Core\Routing\RouteObjectInterface;
use Drupal\tripal\Services\TripalJob;
use Drupal\Entity\User;
use Symfony\Component\HttpFoundation\RedirectResponse;

/**
 * Controller routines for the Tripal Module
 */
class TripalJobController extends ControllerBase{

  /**
   * Constructs the TripalJobController.
   *
   */
  public function __construct() {

  }

  /**
   * Provides the main landing page for managing Jobs.
   */
  public function tripalJobs() {

    // set the breadcrumb
    $breadcrumb = new Breadcrumb();
    $breadcrumb->addLink(Link::fromTextAndUrl('Home',
        Url::fromRoute('<front>')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Administration',
        Url::fromUri('internal:/admin')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Tripal',
        Url::fromUri('internal:/admin/tripal')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Jobs',
        Url::fromUri('internal:/admin/tripal/tripal_jobs')));


    $view = \Drupal\views\Views::getView('tripal_jobs');
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

  /**
   * Cancels a job that is in the waiting state.
   *
   * @param $id
   *   The Job ID.
   */
  public function tripalJobsCancel($id) {
    tripal_cancel_job($id, FALSE);
    return $this->redirect('tripal.jobs');
  }

  /**
   * Submits a job to be run again.
   *
   * @param $id
   *   The Job ID.
   */
  public function tripalJobsRerun($id) {
    tripal_rerun_job($id, FALSE);
    return $this->redirect('tripal.jobs');
  }

  /**
   * Executes a job that is in the queue and waits for it to complete.
   *
   * @param $id
   *   The Job ID.
   */
  public function tripalJobsExecute($id) {
    tripal_execute_job($id, FALSE);
    return $this->redirect('tripal.jobs');
  }

  /**
   * Helper function to build an HTML table from an array
   *
   * @param array $array
   *   The array elements to format.
   * @param string $name
   *   The name of the element to which the table belongs.
   * @return string
   *   The rendered HTML markup for the table.
   */
  private function buildArrayTable($array, $name = '') {
    $markup = '';

    // If the table only has one key then simplify this down for display
    $keys = array_keys($array);
    if (count($keys) == 1) {
      $key = $keys[0];
      return $this->buildArrayTable($array[$key], $key);
    }

    $table = [
      '#type' => 'table',
      '#header' => [
        ['data' => 'Key'],
        ['data' => 'Value'],
      ],
      '#rows' => [],
    ];
    if ($name) {
      $table['#caption'] = $this->t('Values for the "@name" element:', ['@name' => $name]);
    }

    // If the argument is an associative array then create a sub table.
    if(array_keys($array) !== range(0, count($array) - 1)) {
      foreach ($array as $key => $value) {
        if (is_array($value)) {
          $value = $this->buildArrayTable($value, $key);
        }
        $table['#rows'][] = [
          'data' => [
            ['data' => $key],
            ['data' => $value],
          ],
        ];
      }
      $markup = \Drupal::service('renderer')->render($table);
    }
    // Otherwise it's just a normal array.
    else{
      foreach ($array as $key => $value) {
        if (is_array($value)) {
          $value = $this->buildArrayTable($value, $key);
        }
        $table['#rows'][] = [
          'data' => [
            ['data' => $key],
            ['data' => $value],
          ],
        ];
      }
      $markup = \Drupal::service('renderer')->render($table);
    }

    return $markup;
  }

  /**
   * Generates a renderagble array containing the job arguments.
   *
   * @param array $arguments
   */
  private function buildArgList($arguments) {

    $arglist[] = [
      '#type' => 'item',
      '#markup' => $this->buildArrayTable($arguments),
      '#prefix' => '<div class="tripal-job-arg-array-val">',
      '#suffix' => '</div>'
    ];
    return $arglist;
  }

  /**
   * Provides a view of all details for a single job
   *
   * @param $id
   *   The Job ID.
   */
  public function tripalJobsView($id) {

    // Set the breadcrumb.
    $breadcrumb = new Breadcrumb();
    $breadcrumb->addLink(Link::fromTextAndUrl('Home',
        Url::fromRoute('<front>')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Administration',
        Url::fromUri('internal:/admin')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Tripal',
        Url::fromUri('internal:/admin/tripal')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Jobs',
        Url::fromUri('internal:/admin/tripal/tripal_jobs')));

    // Get the Job.
    $job = new TripalJob();
    $job->load($id);

    // Allow the modules to describe their job arguments for the user.
    $arg_hook = $job->getModuleName() . "_job_describe_args";
    $arguments = $job->getArguments();
    if (is_callable($arg_hook)) {
      $new_args = call_user_func_array($arg_hook, [$job->getCallback(), $arguments]);
      if (is_array($new_args) and count($new_args)) {
        $arguments = $new_args;
      }
    }

    // Generate the list of arguments for display.
    $arglist = $this->buildArgList($arguments);

    // Set the title of the page.
    $request = \Drupal::request();
    if ($route = $request->attributes->get(RouteObjectInterface::ROUTE_OBJECT)) {
      $route->setDefault('_title', 'Job Details: ' . $job->getJobName());
    }

    // Build the links array for the dropbutton.
    $links = [];
    $links['return'] = [
      'title' => t('Return to jobs list'),
      'url' => Url::fromUri("internal:/admin/tripal/tripal_jobs/")
    ];
    $links['rerun'] = [
      'title' => t('Re-run this job'),
      'url' => Url::fromUri("internal:/admin/tripal/tripal_jobs/rerun/" . $id,
          ['query' => ['destination' => Url::fromUri('internal:/admin/tripal/tripal_jobs/view/' . $id)->toString()]])
    ];
    if ($job->getStartTime() == 0 and $job->getEndTime() == 0) {
      $links['cancel'] = [
        'title' => t('Cancel this job'),
        'url' => Url::fromUri("internal:/admin/tripal/tripal_jobs/cancel/" . $id,
            ['query' => ['destination' => Url::fromUri('internal:/admin/tripal/tripal_jobs/view/' . $id)->toString()]])
      ];
    }

    // Get the submitter info.
    $submitter = \Drupal\user\Entity\User::load($job->getUID());


    // Build the render array for the table.
    $content = [];
    $content['job_details'] = [
      '#type' => 'table',
      '#header' => [],
      '#rows' => [
        [
          ['header' => TRUE, 'data' => 'Job Name'],
          $job->getJobName()
        ],
        [
          ['header' => TRUE, 'data' => 'Actions'],
          ['data' => [
            '#type' => 'dropbutton',
            '#links' => $links,
          ]],
        ],
        [
          ['header' => TRUE, 'data' => 'Job ID'],
          $id
        ],
        [
          ['header' => TRUE, 'data' => 'Job Status'],
          $job->getStatus()
        ],
        [
          ['header' => TRUE, 'data' => 'Submitting Module'],
          $job->getModuleName()
        ],
        [
          ['header' => TRUE, 'data' => 'Callback function'],
          $job->getCallback()
        ],
        [
          ['header' => TRUE, 'data' => 'Progress'],
          $job->getProgress() . "%"
        ],
        [
          ['header' => TRUE, 'data' => 'Status'],
          $job->getStatus()
        ],
        [
          ['header' => TRUE, 'data' => 'Submit Date'],
          $job->getSubmitTime() ? \Drupal::service('date.formatter')->format($job->getSubmitTime()) : ''
        ],
        [
          ['header' => TRUE, 'data' => 'Start time'],
          $job->getStartTime() ? \Drupal::service('date.formatter')->format($job->getStartTime()) : ''
        ],
        [
          ['header' => TRUE, 'data' => 'End time'],
          $job->getEndTime() ? \Drupal::service('date.formatter')->format($job->getEndTime()) : ''
        ],
        [
          ['header' => TRUE, 'data' => 'Priority'],
          $job->getPriority()
        ],
        [
          ['header' => TRUE, 'data' => 'Submitting User'],
          $submitter->getDisplayName()
        ],
        [
          ['header' => TRUE, 'data' => 'Arguments'],
          ['data' => $arglist]
        ],
        [
          ['header' => TRUE, 'data' => 'Job Log '],
          Markup::create('<pre class="tripal-job-logs">' . $job->getLog() . '</pre>')
        ],
      ],
      // Make sure the CSS for this table is attached.
      '#attached' => [
        'library' => ['tripal/jobs'],
      ]
    ];
    return $content;
  }
}
