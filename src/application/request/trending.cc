#include <application/request.h>

void debug_node(GumboNode *node, std::string indent) {
  for (unsigned int i = 0; i < node->v.element.children.length; ++i) {
    GumboNode *child = static_cast<GumboNode *>(node->v.element.children.data[i]);
    spdlog::info("debug: {} {} {}", indent, child->type, gumbo_normalized_tagname(child->v.element.tag));
  }
}

int Request::search_for_article(GumboNode *node, const TrendingData &trending) {
  if (node->type != GUMBO_NODE_ELEMENT) {
    return EXIT_SUCCESS;
  }

  GumboAttribute *classname;
  if (node->v.element.tag == GUMBO_TAG_ARTICLE && (classname = gumbo_get_attribute(&node->v.element.attributes, "class"))) {
    if (strcmp(classname->value, "Box-row") == 0) {
      std::string user = "";
      std::string repo = "";
      int64_t star = 0;
      for (unsigned int i = 0; i < node->v.element.children.length; ++i) {
        GumboNode *child = static_cast<GumboNode *>(node->v.element.children.data[i]);
        if (child->v.element.tag == GUMBO_TAG_H1) {
          if (child->v.element.children.length < 2) {
            continue;
          }
          GumboNode *a_child = static_cast<GumboNode *>(child->v.element.children.data[1]);
          if (a_child->v.element.children.length < 4) {
            continue;
          }
          GumboNode *user_span_node = static_cast<GumboNode *>(a_child->v.element.children.data[3]);
          if (user_span_node->v.element.children.length < 1) {
            continue;
          }
          GumboNode *user_node = static_cast<GumboNode *>(user_span_node->v.element.children.data[0]);
          user = boost::algorithm::trim_copy(std::string(user_node->v.text.text));
          user = boost::algorithm::trim_right_copy_if(user, boost::is_any_of(" /"));
          GumboNode *repo_node = static_cast<GumboNode *>(a_child->v.element.children.data[4]);
          repo = boost::algorithm::trim_copy(std::string(repo_node->v.text.text));
        }
        if (i == 7) {
          GumboNode *span_node = nullptr;
          if (child->v.element.children.length == 9) {
            span_node = static_cast<GumboNode *>(child->v.element.children.data[7]);
          } else if (child->v.element.children.length == 11) {
            span_node = static_cast<GumboNode *>(child->v.element.children.data[9]);
          } else {
            continue;
          }
          GumboNode *star_node = static_cast<GumboNode *>(span_node->v.element.children.data[2]);
          std::string star_text = boost::algorithm::trim_copy(std::string(star_node->v.text.text));
          std::regex pieces_regex(R"lit(([\d,]*)\sstars\s(this\smonth|today|this\sweek))lit");
          std::smatch result;
          if (std::regex_search(star_text, result, pieces_regex)) {
            std::string stars_str = result[1];
            stars_str = boost::algorithm::replace_all_copy(stars_str, ",", "");
            int64_t star_tmp = std::stoll(boost::algorithm::trim_copy(stars_str));
            if (star_tmp != 0) {
              star = star_tmp;
            }
          }
        }
      }
      if (user.empty() || repo.empty()) {
        return EXIT_SUCCESS;
      }
      std::time_t now = time(0);
      char buffer[32];
      std::strftime(buffer, 32, "%Y/%m/%d", std::localtime(&now));
      std::string now_str = std::string(buffer);
      Trending t = {
          .seq = trending.seq,
          .spoken_language = trending.spoken_language,
          .language = trending.language,
          .owner = user,
          .repo = repo,
          .star = star,
      };
      WRAP_FUNC(this->database->insert_trending(t, now_str));
    }
  }

  GumboVector *children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    this->search_for_article(static_cast<GumboNode *>(children->data[i]), trending);
  }
  return EXIT_SUCCESS;
}

int Request::startup_trending_repos() {
  std::vector<std::string> sequence = {"daily", "weekly", "monthly"};
  std::vector<std::string> spoken_language = {"en", "zh"};
  std::vector<std::string> language = {"c", "c++", "c%23" /* C# */, "go", "java", "javascript", "php", "python", "ruby", "swift", "typescript"};

  for (auto seq : sequence) {
    for (auto lang : spoken_language) {
      for (auto l : language) {
        std::string path = fmt::format("/trending/{}?since={}&spoken_language_code={}", l, seq, lang);
        if (config.crawler_type_trending_repos) {
          semaphore++;
          std::thread trending_repos_thread([=]() {
            spdlog::info("Trending repos thread {} {} {} is starting...", seq, l, lang);
            while (!stopping) {
              std::this_thread::sleep_for(std::chrono::milliseconds(200));
              time_t now = time(0);
              struct tm *ltm = std::localtime(&now);
              if (seq == "daily") {
                if (ltm->tm_hour != 0) {
                  continue;
                }
              } else if (seq == "weekly") {
                if (ltm->tm_wday != 0) {
                  continue;
                }
              } else if (seq == "monthly") {
                if (ltm->tm_mday != 1) {
                  continue;
                }
              }
              if (ltm->tm_min != 0) {
                continue;
              }
              if (ltm->tm_sec != 0) {
                continue;
              }
              RequestConfig request_config{
                  .host = "https://github.com",
                  .path = path,
              };
              request_config.response_type = "xml";
              TrendingData trending_data;
              trending_data.seq = seq;
              trending_data.spoken_language = lang;
              trending_data.language = l;
              request_config.trending = trending_data;
              int code = request(request_config, request_type_orgs, request_type_orgs);
              if (code != 0) {
                spdlog::error("Request url: {} with error: {}", request_config.path, code);
              }
              std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            spdlog::info("Trending repos thread {} {} {} stopped", seq, l, lang);
            semaphore--;
          });
          trending_repos_thread.detach();
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
