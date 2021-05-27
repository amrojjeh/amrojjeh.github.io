// Global variables are defined in data.js

init();

function init() // Initializes the site by loading everything neccessary
{
	loadProjects(projects);
}

function loadProjects(parent) // Loads projects from Projects and displays them according to the category
{
	parent.innerHTML = "";
	for (var project in Projects)
		addProject(Projects[project], parent);
}

function addProject(project, parent) // Adds a project and displays
{
	var html = generateHTML(project.Name, project.Description, project.ImagePath, project.Link, project.Date);
	parent.innerHTML += html;
}

function generateHTML(name, description, imagePath, link, date) // Generates the HTML of a project
{
	var header;
	if (imagePath == "None")
		header = '<div><a href="' + link + '" target="_blank"></a>';
	else
		header = '<div><a href="' + link + '" target="_blank"><img src="' + imagePath + '"></a>';
	var h1 = '<h1><a href="' + link + '" target="_blank">' + name + '</a></h1>';
	var date = "<i>Last updated: " + date + "</i>"
	var p = "<p>" + description + "</p>";
	return header + h1 + date + p + "</div>";
}
