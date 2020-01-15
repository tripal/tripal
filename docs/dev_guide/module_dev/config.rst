
Configuration Variables
=========================

Configuration variables can be done two ways in Drupal 8 depending upon whether you want your variables sync'd between production and development environments. Specifically,

 - Simple configuration (Config API) — Settings that require synchronization between different environments, e.g. site name, slogan, user account settings, etc.
 - Local configuration (State API) — Settings that are more transient or subject to change and which should not be synchronized between environments. e.g. the last cron run, the timestamp for statistics, last update time, etc.

Additional Resources:
 - `Official Drupal Docs: Config API <https://www.drupal.org/docs/8/creating-custom-modules/defining-and-using-your-own-configuration-in-drupal-8>`_
 - `Official Drupal Docs: State API <https://www.drupal.org/docs/8/api/state-api/overview>`_
 - `X-Team: Configuration vs State <https://x-team.com/blog/bytesized-drupal-configuration-or-state/>`_
 - `Antistatique: Drupal 8 - Differences between Configuration API & State API <https://antistatique.net/en/we/blog/2016/06/14/drupal-8-differences-between-configuration-api-state-api>`_
