
function tripal_navigate_field_pager($page) {
    console.log($page);
    jQuery("#ajax-target").load("bio_data/ajax/field_attach/"+$page);
}
