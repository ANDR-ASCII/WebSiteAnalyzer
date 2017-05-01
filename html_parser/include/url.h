#pragma once

#include "headers.h"

//#pragma warning(disable: 4251)

namespace HtmlParser
{

    /**********************************************************************************************
	 *
	 * #1. Object of class Link when parse the supplied address into parts
     * saves the relative path:
     *
     * 1) if passed address is an absolute link with the slash at the beginning of the address.
     *
     * 2) if the passed address is a relative link: in the form in which the parser pulled it out of the tag "a".
     *
     * getRelativeDir()  returns levels of directories to the website root.
     * getRelativePath() returns full path to the file from the website root.
     *
     * Example:
     *
     * If function accepts absolute link: www.site.ru/dir1/dir2/index.php
     * getRelativeDir()  returns - /dir1/dir2/
     * getRelativePath() returns - /dir1/dir2/index.php
     *
     *
     * If function accepts relative link: /dir1/dir2/index.php
     * getRelativeDir()  returns - /dir1/dir2/
     * getRelativePath() returns - /dir1/dir2/index.php
     *
     *
     * If function accepts relative link: dir1/dir2/index.php
     * getRelativeDir()  returns - dir1/dir2/
     * getRelativePath() returns - dir1/dir2/index.php
     *
     *
     * If function accepts relative link: dir1/dir2
     * getRelativeDir()  returns - dir1/dir2
     * getRelativePath() returns - dir1/dir2
     *
     * ===============================================================================================
     *
     * Second case:
     * such a behavior is needed for appropriate handling
     * result address converted by Crawler's functions.
     *
     * For which an important role is played by the first slash in the address
     * obtained by the parser from html page.
     * Thus, if found link with relative path of the following kind:
     *
     *            /folder1[/[folder2[/[index.php[?id=1&cat=1]]]]]
     *
     * It does not matter which address you received this link, it is saved
     * in turn relative links like this for what it is.
     *
     * If in relative link first character not a slash, then
     * key role in conversion right address played
     * relative path from was get this address.
     *
     */

    class UrlParseErrorException : public std::runtime_error
    {
        std::string badUrl_;
    public:
        UrlParseErrorException(const std::string &details, const std::string &badUrl)
                : std::runtime_error(details)
                , badUrl_(badUrl)
                {}

        UrlParseErrorException(const char *details, const char *badUrl)
                : std::runtime_error(details)
                , badUrl_(badUrl)
                {}

        virtual ~UrlParseErrorException() {}

        virtual const char *what() const noexcept
        {
            return ("Crash regex algorithm when parsing URL: " + badUrl_ + "\n"
                    "===============================================================================\n"
                    "Details: " + std::runtime_error::what() + "\n"
                    "###############################################################################\n\n\n").c_str();
        }
    };


	class Url
	{
    public:
        enum class FileType
        {
              Undefined // used by default and for files without extensions
            , Image
            , Archive
            , Document
            , Text
            , Audio
            , Video
            , ExecutableWebFile // all files after which can be character '?' and variables
        };

        Url(bool parseFileCategory = true);
		Url(const char* lnk, bool parseFileCategory = true);
		Url(const std::string& lnk, bool parseFileCategory = true);

		// comparison addresses

		// true if links equivalents
		bool operator==(const Url& other) const;
		bool operator!=(const Url& other) const;

		operator bool() const;

        Url& operator=(const char* link);
		Url& operator=(const std::string& link);

		void clear();

		// check the link
		// before call this method must call clear()
		void parse(const std::string& testLnk);

        // it compares *this host with cmpLnk host
        // if second parameter is true then domain.com and www.domain.com will be equivalent
		bool compareHost(const Url& cmpLnk) const;

        // returns number of element in vector of variables
        // if specify var was found, otherwise -1
		int isSetVariable(const std::string& nameOfVar);

		// set flag for determine content type based on file
		void determineContentType(bool parseFileCategory);

        // returns value of specify variable if success
        // otherwise returns empty string
		std::string variableValue(const std::string& variableName);

		// returns value of variable by number 'variableNumber' if success
        // otherwise returns empty string
		std::string variableValue(std::size_t variableNumber);

        // check the status link
		bool isAnchor() const;
		bool isAbsoluteAddress() const;
		bool isRelativeAddress() const;

		// returns string like this: [www.] [subdomain.] domain.com
		const FileType& fileType() const;
		const std::string& fileExtenstion() const;
		const std::string& protocol() const;
		const std::string& host() const;
		const std::string& relativePath() const;
		const std::string& relativeDirectory() const;
		const std::string& file() const;
		const std::string& anchor() const;
		const std::string& topLevelDomain() const;

		// returns string appropriate get variables: ?variable_1=value_1&variable_2=value_2
		// empty if variables not exists
		const std::string& variablesString() const;

    private:

		// check status link
		// sets if the link is anchor: #name_anchor
		// but not set if anchor occurred in the end of the link
		bool m_anchor;

		// sets if the link is absolute address:
		// [www.] [subdomain.] domain.com [ [/folder1/folder2/] page.php [? [var1=val1&var2=val2] ] ]
		bool m_absoluteAddress; 

		// sets if the link is relative path: [/folder1/folder2/] page.php [? [var1=val1&var2=val2] ]
		bool m_relativeAddress;

		// set flag for determine content type based on file
		bool m_determineContentType;

		// URL parts after parsing
		std::string m_protocol;
		std::string m_host;
		std::string m_path;
		std::string m_relativeDirectory;
		std::string m_file;
		std::string m_anchorName;
        std::string m_fileExtension;
        std::string m_variablesString;
        std::string m_topLevelDomain;
        FileType m_fileType = FileType::Undefined;

        // consists of a name of variable and value of variable
		std::vector<std::pair<std::string, std::string>> m_variables;

		// replaces &amp; to & in relative path
		void ampToSymb();

		/** handling of possible errors **/

        // parsing relative path by parts
        // must be called after handling of possible errors
        void parseRelativePath();
        void parseMethodGetVariables(const std::string& vars);

        // remove anchor from relative path if it exists: path_
        void removeAnchor();

        // static regex objects, for compile once
        static boost::regex s_absoluteLinkRegexPattern;
        static boost::regex s_relativeLinkRegexPattern;
        static boost::regex s_basedOnFileRegexPattern;
        static boost::regex s_basedOnDirsRegexPattern;
        static boost::regex s_slashesRegexPattern;
        static boost::regex s_ampersandRegexPattern;

        // regular expression for test occurrence on matched file(not web page)
        static boost::regex s_archivesRegexPattern;    // for detect archives
        static boost::regex s_imagesRegexPattern;      // for detect images

        static const std::string s_executeWebFileExtensions[];

        // this method need for compare extension on match with extensions
        // of web pages such as - htm, html, php, asp, aspx(execute files)
        // also it need for compare relative link such a - index.html
        // because such a relative link matched for "statPtnAbsoluteLink"
        bool isExtensionMatchWebPage(const std::string& extension);
	};
}