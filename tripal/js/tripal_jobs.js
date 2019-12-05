// Using the closure to map jQuery to $. 
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.tripalJobs = {
    attach: function (context, settings) {

      function tripal_job_progress(percent, element) {
        var bar = $(element);
        var percent_width = percent * bar.width() / 100;
        bar.find('div').animate({ width: percent_width }, 500).html(percent + "% ");
      }
      
      // The progress_percent value is provided by the
      // tripal_jobs_run_job() function as inline code that gets
      // added to the page on load.
      tripal_job_progress(progress_percent, '#tripal-jobs-progress-bar');
    }
  }
}) (jQuery);