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
    $arglist = [];
    foreach ($arguments as $key => $value) {
      if (is_array($value)) {
        $temp = [];
        foreach ($value as $vk => $vv) {
          $temp[] = [
            '#type' => 'item',
            '#title' => $vk,
            '#markup' => is_array($vv) ? print_r($vv) : $vv,
            '#prefix' => '<div class="tripal-job-arg-array-val">',
            '#suffix' => '</div>'
          ];
        }
        $value = render($temp);
      }
      if (is_numeric($key)) {
        $key = 'Arg #' . $key;
      }
      $arglist[] =  [
        '#type' => 'item',
        '#title' => $key,
        '#markup' =>  $value
      ];
    }

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
