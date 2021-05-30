// Global variables are defined in data.js

init();

function init() // Initializes the site by loading everything neccessary
{
	loadProjects(projects);
}

function loadProjects(parent) // Loads projects from Projects and displays them according to the category
{
	parent.innerHTML = "";
	for (var article in Articles)
		addProject(Articles[article], parent);
}

function addProject(project, parent) // Adds a project and displays
{
	var html = generateHTML(project.Name, project.Description, project.Link, project.Date);
	parent.innerHTML += html;
}

function generateHTML(name, description, link, date) // Generates the HTML of a project
{
	var header = '<div><a href="articles/' + link + '/index.html"><img src="articles/' + link + '/cover.png' + '"></a>';
	var h1 = '<h1><a class="underline" href="articles/' + link + '/index.html">' + name + '</a></h1>';
	var date = "<i>Last updated: " + date + "</i>"
	var p = "<p>" + description + "</p>";
	return header + h1 + date + p + "</div>";
}
