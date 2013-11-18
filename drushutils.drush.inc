<?php
/**
 * @file
 * Provides the 'find-callback' Drush command.
 *
 * This custom command returns the name of the module that implements a given
 * callback by path (either a full path, or a partial match).
 */

/**
 * Implements COMMANDFILE_drush_command().
 *
 * @return array
 *   Array of Drush command definitions.
 */
function drushutils_drush_command() {
  $items = array();

  $items['find-callback'] = array(
    'description' => "Finds the module(s) that implements a callback by exact or partial path.",
    'arguments' => array(
      'path' => "The callback path to find (e.g. 'views/ajax')",
    ),
    'options' => array(
      'match' => "One of 'full' or 'partial'. Whether to match on an exact path or a partial path. Defaults to 'full'.",
    ),
    'examples' => array(
      'drush find-callback views/ajax' => "Find modules that implement the exact callback 'views/ajax'.",
      'drush find-callback views/ajax --match=partial' => "Find modules that implement the callbacks containing 'views/ajax'.",
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $items;
}

/**
 * Find Callback command callback.
 */
function drush_drushutils_find_callback() {
  $matches = array();
  $match_type = drush_get_option('match');

  $args = drush_get_arguments();
  $path_to_find = $args[1];

  foreach (module_implements('menu') as $module) {
    $menu_callbacks = module_invoke($module, 'menu');
    if (!empty($menu_callbacks)) {
      foreach ($menu_callbacks as $path => $callback) {
        switch ($match_type) {
          case 'partial':
            if (FALSE !== strpos($path, $path_to_find)) {
              $matches[] = array($module, $path);
            }
            break;

          case 'full':
          default:
            if ($path == $path_to_find) {
              $matches[] = array($module, $path);
              continue 2;
            }
            break;
        }
      }
    }
  }
  _drushutils_output_results($matches, $match_type);
}

/**
 * Find Callback command validation.
 *
 * @return bool
 *   Response from drush_set_error().
 */
function drush_drushutils_find_callback_validate() {
  $args = drush_get_arguments();
  if (1 == count($args)) {
    return drush_set_error('ERROR', dt('The path (or partial path) to search for must be passed as an argument.'));
  }
  return TRUE;
}

/**
 * Outputs the modules and their matched paths.
 *
 * @param array $matches
 *   An array of {module, path} key-pairs.
 *
 * @param string $match_type
 *   The matching type, 'full' or 'partial'.
 */
function _drushutils_output_results($matches, $match_type) {
  if (!isset($match_type)) {
    $match_type = 'full';
  }
  drush_print(('full' != $match_type ? dt('Partial') : dt('Exact')) . ' matches found:');
  drush_print();
  foreach ($matches as $match) {
    drush_print(dt('Module: ') . $match[0]);
    drush_print(dt('Path: ') . $match[1]);
    drush_print();
  }
  drush_print();
}
