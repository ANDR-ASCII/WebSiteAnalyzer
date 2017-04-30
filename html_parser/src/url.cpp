#include "headers.h"
#include "common.h"
#include "url.h"

namespace HtmlParser
{

const std::string Url::s_executeWebFileExtensions[]
{
	"htm",
	"html",
	"php",
	"asp",
	"aspx"
};

boost::regex Url::s_absoluteLinkRegexPattern(
	"^(https?://|//)?(www\\.)?(?:((?:[\\.\\w-]+?\\.)*))?([\\w-]+)\\."
	"((?:[a-z]{3}\\.[a-z]{2})|(?:[a-z]{2}\\.[a-z]{3})|(?:[a-z]{2}\\.[a-z]{2})|[a-z]{2,6})(:\\d+)?(.*)?$",
	boost::regex_constants::icase
);

boost::regex Url::s_relativeLinkRegexPattern(
	"^(?:/|//|\\|\\\\)?((?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?(?:/|//|\\|\\\\)?)*"
	"(?:(?:((?:/|//|\\|\\\\)?(?:[[:space:]\\w\\.\\+\\*~!:;-]|%[0-9a-f]{2})+?)(?:\\.(py|asp|php|htm|html|aspx))?)?"
	"(\\?[[:print:]]+?)?)?(?:#.*)?$",
	boost::regex_constants::icase
);

boost::regex Url::s_basedOnFileRegexPattern(
	"^((?:[/[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})*?)((?:(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+)(?:\\."
	// enumeration extensions of web pages
	"py|asp|php|htm|html|aspx)?)"
	"(?:\\?&?((?:\\w+(?:=?(?:[[:print:]]+)&?)?)*)?)?$",
	boost::regex_constants::icase
);

boost::regex Url::s_basedOnDirsRegexPattern(
	"^((?:/?(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+)*/?)"
	"(?:/\\?&?((?:\\w+(?:=?(?:[[:print:]]+)&?)?)*)?)?$",
	boost::regex_constants::icase
);

boost::regex Url::s_slashesRegexPattern("^/|//|\\|\\\\$");
boost::regex Url::s_ampersandRegexPattern("&amp;");

/** regular expression for test occurrence on matched file(not web page) **/
/** categories of files                                                 **/
/** #1 archives                                                         **/

boost::regex Url::s_archivesRegexPattern(
	"^(?:(?:/|//|\\|\\\\)?(?:(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?)/|//|\\|\\\\)*"
	"(?:/|//|\\|\\\\)?((?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?)\\."
	"("
	// enumeration extensions of archives
	"tar.gz|tar.xz|tar.bz2|tar|tgz|exe|7z|zip|rar|bin|arj|cab|cbr|deb|gzip|jar|one|pak|pkg|ppt|"
	"rpm|sh|sib|sis|sisx|sit|sitx|spl|xar|zipx|gz|xz|bz2|sig|patch"
	")$",
	boost::regex_constants::icase
);

/** #2 images **/
boost::regex Url::s_imagesRegexPattern(
	"^(?:(?:/|//|\\|\\\\)?(?:(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?)/|//|\\|\\\\)*"
	"(?:/|//|\\|\\\\)?(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?\\."
	"jpg|gif|png|bmp|jpeg"
	"$",
	boost::regex_constants::icase
);


Url::Url(bool parseFileCategory)
	: m_determineContentType(parseFileCategory)
{
}

Url::Url(const char* url, bool parseFileCategory)
	: m_determineContentType(parseFileCategory)
{
	parse(url);
}

Url::Url(const std::string& url, bool parseFileCategory)
	: m_determineContentType(parseFileCategory)
{
	parse(url);
}


Url::operator bool() const
{
	return m_anchor || m_absoluteAddress || m_relativeAddress;
}

// true if links equivalents
bool Url::operator==(const Url& other) const
{
	if (&other == this)
	{
		return true;
	}

	if (Common::strToLower(host()) == Common::strToLower(other.host()) &&
		relativeDir() == other.relativeDir() &&
		file() == other.file())
	{
		if (m_variables.size() != other.m_variables.size())
		{
			return false;
		}

		// compare variables
		std::size_t variablesNumber = m_variables.size();

		for (std::size_t i = 0; i < variablesNumber; ++i)
		{
			for (std::size_t j = 0; j < variablesNumber; ++j)
			{
				if (m_variables[i].first == other.m_variables[j].first)
				{
					if (m_variables[i].second == other.m_variables[j].second)
					{
						break;
					}
					else
					{
						return false;
					}
				}

				if (j == variablesNumber - 1)
				{
					return false;
				}
			}

		}

		return true;
	}

	return false;
}

bool Url::operator!=(const Url &cmpUrl) const
{
	if (*this == cmpUrl)
	{
		return false;
	}

	return true;
}

Url& Url::operator=(const char *url)
{
	parse(url);
	return *this;
}

Url& Url::operator=(const std::string &url)
{
	parse(url);
	return *this;
}

void Url::parse(const std::string &testLnk)
{
	clear();

	std::string url = testLnk;
	Common::cutSpaceChars(url);

	if (url.find("javascript:") != std::string::npos || url.find("mailto:") != std::string::npos)
	{
		return;
	}

	try
	{
		if (url.empty())
		{
			return;
		}

		boost::smatch partsOfLink;

		// if it anchor
		if (url[0] == '#')
		{
			m_anchorName = url;
			m_anchor = true;
			return;
		}


		// this is absolute address, if matched pattern and last part after '.' is not a web executing extension
		// because .html extension also match with: [a-z]{2,6}|[a-z]{3}\\.[a-z]{2}|[a-z]{2}\\.[a-z]{3}|[a-z]{2}\\.[a-z]{2}
		m_absoluteAddress = boost::regex_match(url, partsOfLink, s_absoluteLinkRegexPattern) && !isExtensionMatchWebPage(partsOfLink[5]);

		if (m_absoluteAddress)
		{
			m_protocol = (partsOfLink[1].length() && partsOfLink[1] != "//") ? partsOfLink[1] : std::string("http://");
			m_path = partsOfLink[7].length() ? partsOfLink[7] : std::string("/");

			// replace XML present ampersand &amp; to equivalent &
			ampToSymb();

			// conversion to the format: [www.][subdomains]site.com
			m_host = partsOfLink[2] + partsOfLink[3] + partsOfLink[4] + "." + partsOfLink[5];
			m_topLevelDomain = partsOfLink[5];

			// separate directory
			if (m_relativeDir != "/")
			{
				// remove anchor
				removeAnchor();
				parseRelativePath();
			}

			return;
		}

		// ==================================================================================

		// if it relative path
		m_relativeAddress = boost::regex_search(url, s_relativeLinkRegexPattern);

		if (m_relativeAddress)
		{
			// separate directory
			m_path = url;

			// replace XML present ampersand &amp; to equivalent &
			ampToSymb();

			// remove anchor
			removeAnchor();
			parseRelativePath();
		}
	}
	catch (std::runtime_error& rte)
	{
		(void)rte;
		throw;
		// catch exception generated by boost library
		// add reference which throws an exception
		//throw HtmlParser::exErrorUrl(rte.what(), url);
	}
}

// remove anchor from relative path if it exists: path_
// otherwise nothing not do
void Url::removeAnchor()
{
	// remove anchor
	std::string::size_type anchorIndex = m_path.rfind('#');

	if (anchorIndex != std::string::npos)
	{
		m_anchorName.assign(m_path, anchorIndex, std::string::npos);
		m_path.assign(m_path, 0, anchorIndex);
	}
}

// parsing relative path by parts
void Url::parseRelativePath()
{
	boost::smatch firstCmp, sm;

	/**
	** NOTE:
	**      /some  - some is a file without extension.
	**      /some/ - some is a folder.
	**/

	// If file found
	if ((boost::regex_search(m_path, firstCmp, s_basedOnFileRegexPattern)))
	{
		if (m_determineContentType)
		{
			// #1
			if (boost::regex_search(m_path, sm, s_archivesRegexPattern))
			{
				// write name of file with extension
				m_file = sm[1] + '.' + sm[2];
				m_fileExtension = sm[2];

				m_relativeDir.assign(m_path, 0, m_path.find(m_file));
				m_fileType = FileType::Archive;
				return;
			}
			// #2
			else if (boost::regex_search(m_path, sm, s_imagesRegexPattern))
			{
				// write name of file with extension
				m_file = sm[1] + '.' + sm[2];
				m_fileExtension = sm[2];

				m_relativeDir.assign(m_path, 0, m_path.find(m_file));
				m_fileType = FileType::Image;
				return;
			}
		}

		// obtains the directories and get variables
		m_file = boost::regex_replace(m_path, s_basedOnFileRegexPattern, "$2");

		std::string::size_type lbl = std::string::npos;
		if ((lbl = m_file.find(".")) != std::string::npos)
		{
			m_fileExtension.assign(m_file, lbl + 1, std::string::npos);
		}

		if (m_fileType == FileType::Undefined)
		{
			// if after all tests type of extensions not set
			// then set type as EXECUTE_WEB_FILE

			// such behavior by default
			m_fileType = FileType::ExecutableWebFile;
		}

		// if file found but can not be found folders or/and get variables
		m_relativeDir = boost::regex_replace(m_path, s_basedOnFileRegexPattern, "$1");
		m_variablesString = boost::regex_replace(m_path, s_basedOnFileRegexPattern, "$3");

		if (!m_variablesString.empty())
		{
			parseGetVars(m_variablesString);
			m_variablesString = "?" + m_variablesString;
		}

		if (m_relativeDir == m_path)
		{
			m_relativeDir.clear();
		}

	}
	else
	{
		// if file not found

		// if found folder or get variables
		if (boost::regex_search(m_path, sm, s_basedOnDirsRegexPattern))
		{
			m_relativeDir = sm[1];
			m_variablesString = sm[2];

			if (!m_variablesString.empty())
			{
				parseGetVars(m_variablesString);
				m_variablesString = "?" + m_variablesString;
				m_relativeDir.append("/");
			}
		}
		else
		{
			throw std::runtime_error("Error when parsing the relative path.\n"
				"File, folders and GET variables not found.");
		}
	}
}

//
void Url::parseGetVars(const std::string &vars)
{
	boost::regex exp("(.*?)=(.*)");

	std::string::size_type label = vars.find('&'), begin = 0, size = vars.size();

	while (label != std::string::npos)
	{
		std::string var_val = vars.substr(begin, label - begin);
		begin = ++label;

		// if var have value
		if (boost::regex_search(var_val, exp))
		{
			std::string tmpVarName, tmpVarValue;
			tmpVarName = boost::regex_replace(var_val, exp, "$1");
			tmpVarValue = boost::regex_replace(var_val, exp, "$2");

			m_variables.push_back(std::make_pair(Common::cutSpaceChars(tmpVarName), Common::cutSpaceChars(tmpVarValue)));
		}

		label = vars.find('&', begin);
	}

	std::string var_val = vars.substr(begin, std::string::npos);
	// if var have value
	if (boost::regex_search(var_val, exp))
	{
		std::string tmpVarName, tmpVarValue;
		tmpVarName = boost::regex_replace(var_val, exp, "$1");
		tmpVarValue = boost::regex_replace(var_val, exp, "$2");

		m_variables.push_back(std::make_pair(Common::cutSpaceChars(tmpVarName), Common::cutSpaceChars(tmpVarValue)));
	}
}

void Url::clear()
{
	m_protocol.clear();
	m_host.clear();
	m_path.clear();
	m_relativeDir.clear();
	m_file.clear();
	m_anchorName.clear();
	m_variables.clear();

	// reset flags
	m_anchor = m_absoluteAddress = m_relativeAddress = false;
}

// it compares *this host with cmpLnk host
// if second parameter is true then subdomain.domain.com and www.subdomain.domain.com will be equivalent
bool Url::compareHost(const Url &cmpLnk) const
{
	if (&cmpLnk == this)
	{
		return true;
	}

	if (Common::strToLower(m_host) == Common::strToLower(cmpLnk.m_host))
	{
		return true;
	}

	return false;
}

// replaces &amp; to & in relative path
void Url::ampToSymb()
{
	if (!m_path.empty())
	{
		m_path = boost::regex_replace(m_path, s_ampersandRegexPattern, "&");
	}
}

// this method need for compare extension on match with extensions
// of web pages such as - htm, html, php, asp, aspx(execute files)
bool Url::isExtensionMatchWebPage(const std::string &extension)
{
	for (const auto &ext : s_executeWebFileExtensions)
	{
		if (ext == extension)
		{
			return true;
		}
	}

	return false;
}

// true if specify variable exists otherwise false
int Url::isSetVariable(const std::string &nameOfVar)
{
	auto size = m_variables.size();
	for (decltype(size) i = 0; i < size; ++i)
	{
		if (m_variables[i].first == nameOfVar)
		{
			return i;
		}
	}

	return -1;
}


// returns value of specify variable if success
// otherwise returns empty string
std::string Url::variableValue(const std::string &nameOfVar)
{
	for (const auto &somePair : m_variables)
	{
		if (somePair.first == nameOfVar)
		{
			return somePair.second;
		}
	}

	return "";
}

// returns value of variable by number 'num' if success
// otherwise returns empty string
std::string Url::variableValue(std::size_t num)
{
	auto size = m_variables.size();

	if (num > size)
	{
		return "";
	}

	return m_variables[num].second;
}

void Url::determineContentType(bool determineContentTypeFlag)
{
	m_determineContentType = determineContentTypeFlag;
}

bool Url::isAnchor() const
{
	return m_anchor;
}

bool Url::isAbsoluteAddress() const
{
	return m_absoluteAddress;
}

bool Url::isRelativeAddress() const
{
	return m_relativeAddress;
}

const Url::FileType& Url::fileType() const
{
	return m_fileType;
}

const std::string& Url::fileExtenstion() const
{
	return m_fileExtension;
}

const std::string& Url::protocol() const
{
	return m_protocol;
}

const std::string& Url::host() const
{
	return m_host;
}

const std::string& Url::relativePath() const
{
	return m_path;
}

const std::string& Url::relativeDir() const
{
	return m_relativeDir;
}

const std::string& Url::file() const
{
	return m_file;
}

const std::string& Url::anchor() const
{
	return m_anchorName;
}

const std::string& Url::topLevelDomain() const
{
	return m_topLevelDomain;
}

const std::string& Url::variablesString() const
{
	return m_variablesString;
}

}