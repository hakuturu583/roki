/* RoKi - Robot Kinetics library
 * Copyright (C) 1998 Tomomichi Sugihara (Zhidao)
 *
 * rk_contact - physical properties of contact
 */

#include <roki/rk_contact.h>

/* ********************************************************** */
/* CLASS: rkContactInfo
 * contact model class
 * ********************************************************** */

static bool _rkContactInfoFRead(FILE *fp, void *instance, char *buf, bool *success);

/* create a rigid contact model. */
rkContactInfo *rkContactInfoRigidCreate(rkContactInfo *ci, double k, double l, double sf, double kf, char *stf1, char *stf2)
{
  rkContactInfoSetType( ci, RK_CONTACT_RIGID );
  rkContactInfoSetK( ci, k );
  rkContactInfoSetL( ci, l );
  rkContactInfoSetSF( ci, sf );
  rkContactInfoSetKF( ci, kf );
  ci->__stf[0] = zStrClone( stf1 );
  ci->__stf[1] = zStrClone( stf2 );
  return ci;
}

/* create an elastic contact model. */
rkContactInfo *rkContactInfoElasticCreate(rkContactInfo *ci, double e, double v, double sf, double kf, char *stf1, char *stf2)
{
  rkContactInfoSetType( ci, RK_CONTACT_ELASTIC );
  rkContactInfoSetE( ci, e );
  rkContactInfoSetV( ci, v );
  rkContactInfoSetSF( ci, sf );
  rkContactInfoSetKF( ci, kf );
  ci->__stf[0] = zStrClone( stf1 );
  ci->__stf[1] = zStrClone( stf2 );
  return ci;
}

/* associate contact info with a pair of stuff. */
rkContactInfo *rkContactInfoAssoc(rkContactInfo *ci, char *stf1, char *stf2)
{
  if( ( strcmp( ci->__stf[0], stf1 ) == 0 && strcmp( ci->__stf[1], stf2 ) == 0 ) ||
      ( strcmp( ci->__stf[0], stf2 ) == 0 && strcmp( ci->__stf[1], stf1 ) == 0 ) )
    return ci;
  return NULL;
}

/* read information of a contact model (internal operation). */
bool _rkContactInfoFRead(FILE *fp, void *instance, char *buf, bool *success)
{
  rkContactInfo *ci;

  ci = instance;
  if( strcmp( buf, "bind" ) == 0 ){
    ci->__stf[0] = zStrClone( zFToken( fp, buf, BUFSIZ ) );
    ci->__stf[1] = zStrClone( zFToken( fp, buf, BUFSIZ ) );
  } else if( strcmp( buf, "staticfriction" ) == 0 ){
    rkContactInfoSetSF( ci, zFDouble( fp ) );
  } else if( strcmp( buf, "kineticfriction" ) == 0 ){
    rkContactInfoSetKF( ci, zFDouble( fp ) );
  } else if( strcmp( buf, "compensation" ) == 0 ){
    rkContactInfoSetK( ci, zFDouble( fp ) );
    rkContactInfoSetType( ci, RK_CONTACT_RIGID );
  } else if( strcmp( buf, "relaxation" ) == 0 ){
    rkContactInfoSetL( ci, zFDouble( fp ) );
    rkContactInfoSetType( ci, RK_CONTACT_RIGID );
  } else if( strcmp( buf, "elasticity" ) == 0 ){
    rkContactInfoSetE( ci, zFDouble( fp ) );
    rkContactInfoSetType( ci, RK_CONTACT_ELASTIC );
  } else if( strcmp( buf, "viscosity" ) == 0 ){
    rkContactInfoSetV( ci, zFDouble( fp ) );
    rkContactInfoSetType( ci, RK_CONTACT_ELASTIC );
  } else
    return false;
  return true;
}

/* read information of a contact model. */
rkContactInfo *rkContactInfoFRead(FILE *fp, rkContactInfo *ci)
{
  rkContactInfoInit( ci );
  zFieldFRead( fp, _rkContactInfoFRead, ci );
  if( ci->__stf[0] && ci->__stf[1] ) return ci;
  ZRUNERROR( RK_ERR_CONTACT_UNBOUND );
  return NULL;
}

/* write information of a contact model. */
void rkContactInfoFWrite(FILE *fp, rkContactInfo *ci)
{
  fprintf( fp, "bind: %s %s\n", ci->__stf[0], ci->__stf[1] );
  switch( rkContactInfoType(ci) ){
  case RK_CONTACT_RIGID:
    fprintf( fp, "compensation: %.10g\n", rkContactInfoK(ci) );
    fprintf( fp, "relaxation: %.10g\n", rkContactInfoL(ci) );
    break;
  case RK_CONTACT_ELASTIC:
    fprintf( fp, "elasticity: %.10g\n", rkContactInfoE(ci) );
    fprintf( fp, "viscosity: %.10g\n", rkContactInfoV(ci) );
    break;
  default: ;
  }
  fprintf( fp, "staticfriction: %.10g\n", rkContactInfoSF(ci) );
  fprintf( fp, "kineticfriction: %.10g\n", rkContactInfoKF(ci) );
  fprintf( fp, "\n" );
}

/* ********************************************************** */
/* CLASS: rkContactInfoPool
 * contact model pool class
 * ********************************************************** */

static bool _rkContactInfoPoolFRead(FILE *fp, void *instance, char *buf, bool *success);

/* destroy contact info ppool */
void rkContactInfoPoolDestroy(rkContactInfoPool *ci)
{
  register int i;

  if( !ci ) return;
  for( i=0; i<zArraySize(ci); i++ )
    rkContactInfoDestroy( zArrayElemNC(ci,i) );
  zArrayFree( ci );
}

/* associate contact info with a pair of keys. */
rkContactInfo *rkContactInfoPoolAssoc(rkContactInfoPool *ci, char *stf1, char *stf2)
{
  register int i;

  if( !stf1 || !stf2) return NULL;
  for( i=0; i<zArraySize(ci); i++ )
    if( rkContactInfoAssoc( zArrayElemNC(ci,i), stf1, stf2 ) )
      return zArrayElemNC(ci,i);
  return NULL;
}

/* associate contact info that matches specified type with a pair of keys. */
rkContactInfo *rkContactInfoPoolAssocType(rkContactInfoPool *ci, char *stf1, char *stf2, char type)
{
  register int i;

  if( !stf1 || !stf2) return NULL;

  for( i=0; i<zArraySize(ci); i++ )
    if( rkContactInfoType( zArrayElemNC(ci,i) ) == type && rkContactInfoAssoc( zArrayElemNC(ci,i), stf1, stf2 ) )
      return zArrayElemNC(ci,i);
  return NULL;
}

/* read contact info pool from file. */
bool rkContactInfoPoolReadFile(rkContactInfoPool *ci, char filename[])
{
  FILE *fp;
  rkContactInfoPool *result;

  if( !( fp = zOpenZTKFile( filename, "r" ) ) )
    return false;
  result = rkContactInfoPoolFRead( fp, ci );
  fclose( fp );
  return result != NULL;
}

typedef struct{
  rkContactInfoPool *ci;
  int c;
} _rkContactInfoPoolParam;

bool _rkContactInfoPoolFRead(FILE *fp, void *instance, char *buf, bool *success)
{
  _rkContactInfoPoolParam *prm;
  rkContactInfo *ci;

  prm = instance;
  ci = zArrayElemNC( prm->ci, prm->c++ );
  if( strcmp( buf, RK_CONTACTINFO_TAG ) == 0 )
    if( !rkContactInfoFRead( fp, ci ) )
      return ( *success = false );
  if( rkContactInfoPoolAssoc( prm->ci, ci->__stf[0], ci->__stf[1] ) != ci )
    ZRUNWARN( RK_WARN_CONTACT_DUPKEY, ci->__stf[0], ci->__stf[1] );
  return true;
}

/* read contact info pool from file. */
rkContactInfoPool *rkContactInfoPoolFRead(FILE *fp, rkContactInfoPool *ci)
{
  _rkContactInfoPoolParam prm;

  zArrayAlloc( ci, rkContactInfo, zFCountTag( fp, RK_CONTACTINFO_TAG ) );
  if( !zArrayBuf(ci) ){
    ZALLOCERROR();
    return NULL;
  }
  prm.ci = ci;
  prm.c = 0;
  if( zTagFRead( fp, _rkContactInfoPoolFRead, &prm ) )
    return ci;
  zArrayFree( ci );
  return NULL;
}

/* write contact info pool to file. */
bool rkContactInfoPoolWriteFile(rkContactInfoPool *ci, char filename[])
{
  char name[BUFSIZ];
  FILE *fp;

  if( !( fp = zOpenZTKFile( name, "w" ) ) ) return false;
  rkContactInfoPoolFWrite( fp, ci );
  return true;
}

/* write contact info pool to file. */
void rkContactInfoPoolFWrite(FILE *fp, rkContactInfoPool *ci)
{
  register int i;

  for( i=0; i<zArraySize(ci); i++ ){
    fprintf( fp, "[%s]\n", RK_CONTACTINFO_TAG );
    rkContactInfoFWrite( fp, zArrayElemNC(ci,i) );
  }
}
