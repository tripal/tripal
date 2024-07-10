var chadoTable = document.getElementsByClassName('chadoTableButton');
for (i = 0; i < chadoTable.length; i++) {
  chadoTable[i].addEventListener('click', submitToTaskFrom, false);
}
function submitToTaskFrom(event) {
  event.preventDefault();
  var chado_form = this.closest("form");
  var schema = chado_form.querySelector("input[name=chado_schema]");
  var task = chado_form.querySelector("input[name=task]");
  task.value = this.dataset.chadoTask;
  schema.value = this.dataset.chadoSchema;
  chado_form.submit();
  return false;
}
