#include "erl_nif.h"
#include "sepa.h"
#include <string.h>

static ErlNifResourceType *SEPA_MSG;

/**
 * Struct to contain the pointer to a sepa message
 * This will be sent back end forth with the beam in form of a resource.
 */
typedef struct
{
  sepa_t *msg;
} sepa_msg;

/**
 * Destructor the sepa_msg resources
 * Free the sepa_t value.
 */
void sepa_destructor(ErlNifEnv *env, void *res)
{
  sepa_msg *sepa = (sepa_msg *)res;
  sepa_free(sepa->msg);
}

/**
 * Initialize callback for nifs
 * Initializes the sepa_msg resource type.
 */
int load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info)
{
  int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
  ErlNifResourceType *msg = enif_open_resource_type(env, NULL, "sepa_msg", sepa_destructor, flags, NULL);
  if (!msg)
    return -1;
  SEPA_MSG = msg;
  return 0;
}

/**
 * Wrapped library functions
 */
static ERL_NIF_TERM
create_message(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  long msgtype;
  sepa_msg *sepa;

  if (argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_long(env, argv[0], &msgtype))
  {
    return enif_make_badarg(env);
  }

  if (msgtype != SEPA_MSGTYPE_DDI && msgtype != SEPA_MSGTYPE_CTI)
  {
    return enif_make_badarg(env);
  }

  sepa = enif_alloc_resource(SEPA_MSG, sizeof(sepa_msg));
  sepa->msg = sepa_new(msgtype);

  if (sepa->msg != NULL)
  {
    ERL_NIF_TERM term = enif_make_resource(env, sepa);
    enif_release_resource(sepa);
    return enif_make_tuple2(env, enif_make_atom(env, "ok"), term);
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

static ERL_NIF_TERM
set_name(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  sepa_msg *sepa;
  char name[70 + 1];
  sepa_status_t st;

  if (argc != 2)
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_resource(env, argv[0], SEPA_MSG, (void **)&sepa))
  {
    return enif_make_badarg(env);
  }

  if (enif_get_string(env, argv[1], name, sizeof(name), ERL_NIF_LATIN1) < 0)
  {
    return enif_make_badarg(env);
  }

  st = sepa_setName(sepa->msg, name);

  if (st == SEPA_OK)
  {
    return enif_make_atom(env, "ok");
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

static ERL_NIF_TERM
set_iban(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  sepa_msg *sepa;
  char iban[SEPA_IBAN_MAXLENGTH + 1];
  sepa_status_t st;

  if (argc != 2)
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_resource(env, argv[0], SEPA_MSG, (void **)&sepa))
  {
    return enif_make_badarg(env);
  }

  if (enif_get_string(env, argv[1], iban, sizeof(iban), ERL_NIF_LATIN1) < 0)
  {
    return enif_make_badarg(env);
  }

  st = sepa_setIBAN(sepa->msg, iban);

  if (st == SEPA_OK)
  {
    return enif_make_atom(env, "ok");
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

static ERL_NIF_TERM
set_bic(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  sepa_msg *sepa;
  char bic[SEPA_BIC_LENGTH + 1];
  sepa_status_t st;

  if (argc != 2)
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_resource(env, argv[0], SEPA_MSG, (void **)&sepa))
  {
    return enif_make_badarg(env);
  }

  if (enif_get_string(env, argv[1], bic, sizeof(bic), ERL_NIF_LATIN1) < 0)
  {
    return enif_make_badarg(env);
  }

  st = sepa_setBIC(sepa->msg, bic);

  if (st == SEPA_OK)
  {
    return enif_make_atom(env, "ok");
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

static ERL_NIF_TERM
add(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  sepa_msg *sepa;
  size_t count;
  int i = 0;
  sepa_keyvalue_t *kv;
  sepa_status_t st;
  ERL_NIF_TERM key, value;
  ErlNifMapIterator iter;
  char **array;

  if (argc != 2)
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_resource(env, argv[0], SEPA_MSG, (void **)&sepa))
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_map_size(env, argv[1], &count))
  {
    return enif_make_badarg(env);
  }

  kv = (sepa_keyvalue_t *)malloc((count + 1) * sizeof(sepa_keyvalue_t));
  array = malloc((count * 2) * sizeof(char *));

  enif_map_iterator_create(env, argv[1], &iter, ERL_NIF_MAP_ITERATOR_FIRST);

  while (enif_map_iterator_get_pair(env, &iter, &key, &value))
  {
    int kindex = i * 2;
    int vindex = i * 2 + 1;
    array[kindex] = (char *)malloc(500 + 1);
    array[vindex] = (char *)malloc(500 + 1);

    if (enif_get_string(env, key, array[kindex], 500, ERL_NIF_LATIN1) < 0)
    {
      return enif_make_badarg(env);
    }
    if (enif_get_string(env, value, array[vindex], 500, ERL_NIF_LATIN1) < 0)
    {
      return enif_make_badarg(env);
    }

    kv[i].key = array[kindex];
    kv[i].value = array[vindex];

    i++;
    enif_map_iterator_next(env, &iter);
  }
  enif_map_iterator_destroy(env, &iter);

  kv[i].key = NULL;
  kv[i].value = NULL;

  st = sepa_add(sepa->msg, kv);

  free(kv);

  if (st == SEPA_OK)
  {
    return enif_make_atom(env, "ok");
  }
  else
  {
    char *err = (char *)sepa_getError(sepa->msg);
    ERL_NIF_TERM error = enif_make_string(env, err, ERL_NIF_LATIN1);
    return enif_make_tuple2(env, enif_make_atom(env, "error"), error);
  }
}

static ERL_NIF_TERM
to_xml(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  sepa_msg *sepa;
  char *xml;

  if (argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (!enif_get_resource(env, argv[0], SEPA_MSG, (void **)&sepa))
  {
    return enif_make_badarg(env);
  }

  xml = sepa_toXML(sepa->msg);

  if (xml != NULL)
  {
    ERL_NIF_TERM ret = enif_make_string(env, xml, ERL_NIF_LATIN1);
    free(xml);
    return enif_make_tuple2(env, enif_make_atom(env, "ok"), ret);
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

static ERL_NIF_TERM
check_iban(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary iban;
  sepa_lookup_status_t ls;

  if (argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (enif_inspect_binary(env, argv[0], &iban) == 0)
  {
    return enif_make_badarg(env);
  }

  ls = sepa_iban_check((char *)iban.data);

  enif_release_binary(&iban);

  if (ls == SEPA_LOOKUP_OK)
  {
    return enif_make_atom(env, "true");
  }
  else
  {
    return enif_make_atom(env, "false");
  }
}

static ERL_NIF_TERM
bic_get_bank_name(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary bic;
  ErlNifBinary bank_name;
  sepa_bankinfo_t *bank;
  sepa_status_t st;

  if (argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (enif_inspect_binary(env, argv[0], &bic) == 0)
  {
    return enif_make_badarg(env);
  }

  st = sepa_bic_getBank((const char *)bic.data, &bank);

  enif_release_binary(&bic);

  if (st == SEPA_OK)
  {
    enif_alloc_binary(strlen(bank->name), &bank_name);
    memcpy(bank_name.data, bank->name, strlen(bank->name));
    free(bank);
    ERL_NIF_TERM term = enif_make_binary(env, &bank_name);
    return enif_make_tuple2(env, enif_make_atom(env, "ok"), term);
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

static ERL_NIF_TERM
iban_convert(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary country_code;
  ErlNifBinary account_id;
  ErlNifBinary bank_id;
  ErlNifBinary iban;
  sepa_lookup_status_t ls;
  char iban_val[SEPA_IBAN_MAXLENGTH + 1];
  char status;

  if (argc != 3)
  {
    return enif_make_badarg(env);
  }

  if (enif_inspect_binary(env, argv[0], &country_code) == 0)
  {
    return enif_make_badarg(env);
  }

  if (enif_inspect_binary(env, argv[1], &account_id) == 0)
  {
    return enif_make_badarg(env);
  }

  if (enif_inspect_binary(env, argv[2], &bank_id) == 0)
  {
    return enif_make_badarg(env);
  }

  ls = sepa_iban_convert(
      (const char *)country_code.data,
      (const char *)account_id.data,
      (const char *)bank_id.data,
      (char *)&iban_val,
      &status);

  enif_release_binary(&country_code);
  enif_release_binary(&account_id);
  enif_release_binary(&bank_id);

  if (ls == SEPA_LOOKUP_OK)
  {
    enif_alloc_binary(strlen(iban_val), &iban);
    memcpy(iban.data, iban_val, strlen(iban_val));
    ERL_NIF_TERM term_1 = enif_make_binary(env, &iban);
    ERL_NIF_TERM term_2 = enif_make_long(env, status);
    return enif_make_tuple3(env, enif_make_atom(env, "ok"), term_1, term_2);
  }
  else
  {
    return enif_make_atom(env, "error");
  }
}

/**
 * NIF Setup
 * Map functions to Sepa.Nif module
 * Setup load callback
 */
static ErlNifFunc nif_funcs[] = {
    {"create_message", 1, create_message, 0},
    {"set_name", 2, set_name, 0},
    {"set_iban", 2, set_iban, 0},
    {"set_bic", 2, set_bic, 0},
    {"add", 2, add, 0},
    {"to_xml", 1, to_xml, 0},
    {"check_iban", 1, check_iban, 0},
    {"bic_get_bank_name", 1, bic_get_bank_name, 0},
    {"iban_convert", 3, iban_convert, 0}};

ERL_NIF_INIT(Elixir.Sepa.Nif, nif_funcs, &load, NULL, NULL, NULL)
